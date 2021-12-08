/*

Copyright (C) 2021, vitasdk

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#include <reent.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/dirent.h>
#include <sys/stat.h>
#include <sys/syslimits.h>
#include <string.h>

#include <psp2/types.h>
#include <psp2/io/stat.h>
#include "vitaerror.h"
#include "vitadescriptor.h"
#include "vitafs.h"

static char __cwd[PATH_MAX] = {0};

int mkdir(const char *pathname, mode_t mode)
{
	return _mkdir_r(_REENT, pathname, mode);
}

int rmdir(const char *pathname)
{
	int ret;
	char* full_path = __realpath(pathname);
	if(!full_path)
	{
		// errno is set by __realpath
		return -1;
	}

	if ((ret = sceIoRmdir(full_path)) < 0)
	{
		free(full_path);
		errno = __vita_sce_errno_to_errno(ret, ERROR_GENERIC);
		return -1;
	}
	free(full_path);
	errno = 0;
	return 0;
}

int lstat(const char *path, struct stat *buf)
{
	return _stat_r(_REENT, path, buf);
}

int chmod(const char *pathname, mode_t mode)
{
	struct SceIoStat stat = {0};
	char* full_path = __realpath(pathname);
	if(!full_path)
	{
		// errno is set by __realpath
		return -1;
	}
	int ret = sceIoGetstat(full_path, &stat);
	if (ret < 0)
	{
		free(full_path);
		errno = __vita_sce_errno_to_errno(ret, ERROR_GENERIC);
		return -1;
	}

	stat.st_mode = mode;
	ret = sceIoChstat(full_path, &stat, SCE_CST_MODE);
	if (ret < 0)
	{
		free(full_path);
		errno = __vita_sce_errno_to_errno(ret, ERROR_GENERIC);
		return -1;
	}
	free(full_path);
	return 0;
}

int fchmod(int fd, mode_t mode)
{
	struct SceIoStat stat = {0};
	int ret = 0;

	DescriptorTranslation *fdmap = __vita_fd_grab(fd);

	if (!fdmap)
	{
		errno = EBADF;
		return -1;
	}

	switch (fdmap->type)
	{
		case VITA_DESCRIPTOR_TTY:
		case VITA_DESCRIPTOR_FILE:
		case VITA_DESCRIPTOR_DIRECTORY:
			ret = sceIoGetstatByFd(fdmap->sce_uid, &stat);
			break;
		case VITA_DESCRIPTOR_SOCKET:
			ret = __vita_make_sce_errno(EBADF);
			break;
	}

	if (ret < 0)
	{
		__vita_fd_drop(fdmap);
		errno = __vita_sce_errno_to_errno(ret, ERROR_GENERIC);
		return -1;
	}

	stat.st_mode = mode;
	ret = sceIoChstatByFd(fdmap->sce_uid, &stat, SCE_CST_MODE);

	__vita_fd_drop(fdmap);

	if (ret < 0)
	{
		errno = __vita_sce_errno_to_errno(ret, ERROR_GENERIC);
		return -1;
	}

	return 0;
}

int fsync(int fd)
{
	int ret = 0;

	DescriptorTranslation *fdmap = __vita_fd_grab(fd);

	if (!fdmap)
	{
		errno = EBADF;
		return -1;
	}

	switch (fdmap->type)
	{
		case VITA_DESCRIPTOR_DIRECTORY:
		case VITA_DESCRIPTOR_FILE:
			ret = sceIoSyncByFd(fdmap->sce_uid, 0);
			break;
		case VITA_DESCRIPTOR_TTY:
		case VITA_DESCRIPTOR_SOCKET:
			ret = __vita_make_sce_errno(EINVAL);
			break;
	}

	__vita_fd_drop(fdmap);

	if (ret < 0)
	{
		errno = __vita_sce_errno_to_errno(ret, ERROR_GENERIC);
		return -1;
	}
	return 0;
}

int fdatasync(int fd)
{
	return fsync(fd);
}

// cwd impl

// get end of drive position from full path
static int __get_drive(const char *path)
{
	int i;
	for(i=0; path[i]; i++) {
		if(! ((path[i] >= 'a' && path[i] <= 'z') || (path[i] >= '0' && path[i] <= '9') ))
			break;
	}

	if(path[i] == ':') return i+1;
	return 0;
}

static void __init_cwd()
{
	if (strlen(__cwd) == 0)
	{
		// init cwd
		strcpy(__cwd, "app0:");
	}
}

char *getcwd(char *buf, size_t size)
{
	__init_cwd();

	if (buf != NULL && size == 0)
	{
		errno = EINVAL;
		return NULL;
	}

	if(buf && strlen(__cwd) >= size) {
		errno = ERANGE;
		return NULL;
	}

	// latest POSIX says it's implementation-defined
	// musl, glibc and bionic allocs buffer if it's null.
	// so we'll do the same
	if (buf == NULL)
	{
		buf = (char*)calloc(PATH_MAX, sizeof(char));
		if (buf == NULL)
		{
			errno = ENOMEM;
			return NULL;
		}
	}

	strcpy(buf, __cwd);
	return buf;
}

// modified from bionic
char *__resolve_path(const char *path, char resolved[PATH_MAX])
{
	char *p, *q, *s;
	size_t left_len, resolved_len;
	char left[PATH_MAX], next_token[PATH_MAX];
	if (path[0] == '/')
	{
		resolved[0] = '/';
		resolved[1] = '\0';
		if (path[1] == '\0')
			return (resolved);
		resolved_len = 1;
		left_len = strlcpy(left, path + 1, sizeof(left));
	}
	else
	{
		resolved_len = 0;
		left_len = strlcpy(left, path, sizeof(left));
	}
	if (left_len >= sizeof(left) || resolved_len >= PATH_MAX)
	{
		errno = ENAMETOOLONG;
		return (NULL);
	}
	/*
	 * Iterate over path components in `left'.
	 */
	while (left_len != 0)
	{
		/*
		 * Extract the next path component and adjust `left'
		 * and its length.
		 */
		p = strchr(left, '/');
		s = p ? p : left + left_len;
		if (s - left >= sizeof(next_token))
		{
			errno = ENAMETOOLONG;
			return (NULL);
		}
		memcpy(next_token, left, s - left);
		next_token[s - left] = '\0';
		left_len -= s - left;
		if (p != NULL)
			memmove(left, s + 1, left_len + 1);
		if (resolved[resolved_len - 1] != '/')
		{
			if (resolved_len + 1 >= PATH_MAX)
			{
				errno = ENAMETOOLONG;
				return (NULL);
			}
			resolved[resolved_len++] = '/';
			resolved[resolved_len] = '\0';
		}
		if (next_token[0] == '\0')
			continue;
		else if (strcmp(next_token, ".") == 0)
			continue;
		else if (strcmp(next_token, "..") == 0)
		{
			/*
			 * Strip the last path component except when we have
			 * single "/"
			 */
			if (resolved_len > 1) {
				resolved[resolved_len - 1] = '\0';
				q = strrchr(resolved, '/') + 1;
				*q = '\0';
				resolved_len = q - resolved;
			}
			continue;
		}
		/*
		 * Append the next path component. 
		 */
		resolved_len = strlcat(resolved, next_token, PATH_MAX);
		if (resolved_len >= PATH_MAX)
		{
			errno = ENAMETOOLONG;
			return (NULL);
		}
	}
	/*
	 * Remove trailing slash except when the resolved pathname
	 * is a single "/".
	 */
	if (resolved_len > 1 && resolved[resolved_len - 1] == '/')
		resolved[resolved_len - 1] = '\0';
	return (resolved);
}

// internal, without stat check. Used for mkdir/open/etc.
char *__realpath(const char *path)
{
	char resolved[PATH_MAX] = {0};
	char result[PATH_MAX] = {0};
	char *resolved_path = NULL;

	// can't have null path
	if (!path)
	{
		errno = EINVAL;
		return NULL;
	}

	// empty path would resolve to cwd
	// POSIX.1-2008 states that ENOENT should be returned if path points to empty string
	if (strlen(path) == 0)
	{
		errno = ENOENT;
		return NULL;
	}

	resolved_path = (char*)calloc(PATH_MAX, sizeof(char));
	if (!resolved_path)
	{
		errno = ENOMEM;
		return NULL;
	}

	int d = __get_drive(path);

	if (d) // absolute path with drive
	{
		__resolve_path(path + d, resolved);
		if (strlen(resolved) + d < PATH_MAX)
		{
			strncpy(result, path, d);
			strcat(result, resolved);
			strcpy(resolved_path, result);
			return resolved_path;
		}
		errno = ENAMETOOLONG;
		free(resolved_path);
		return NULL;
	}
	else if (path[0] == '/') // absolute path without drive
	{
		__init_cwd();
		__resolve_path(path, resolved);
		d = __get_drive(__cwd);
		if (strlen(resolved) + d < PATH_MAX)
		{
			strncpy(result, __cwd, d);
			strcat(result, resolved);
			strcpy(resolved_path, result);
			return resolved_path;
		}
		errno = ENAMETOOLONG;
		free(resolved_path);
		return NULL;
	}
	else // relative path
	{
		__init_cwd();
		char full_path[PATH_MAX] = {0};
		if (strlen(__cwd) + strlen(path) < PATH_MAX)
		{
			strcpy(full_path, __cwd);
			strcat(full_path, "/");
			strcat(full_path, path);
			d = __get_drive(full_path);

			__resolve_path(full_path + d, resolved);
			if (strlen(resolved) + d < PATH_MAX)
			{
				strncpy(result, full_path, d);
				strcat(result, resolved);
				strcpy(resolved_path, result);
				return resolved_path;
			}
			errno = ENAMETOOLONG;
			free(resolved_path);
			return NULL;
		}
		errno = ENAMETOOLONG;
		free(resolved_path);
		return NULL;
	}
}

int __is_dir(const char *path)
{
	SceIoStat stat;
	int ret = sceIoGetstat(path, &stat);
	if (ret < 0)
	{
		return ret;
	}
	if (!SCE_S_ISDIR(stat.st_mode))
	{
		return __vita_make_sce_errno(ENOTDIR);
	}
	return 0;
}

char *realpath(const char *path, char* resolved_path)
{
	char *fullpath = __realpath(path);
	if (!fullpath)
	{
		return NULL; // errno already set
	}

	SceIoStat stat;
	int ret = sceIoGetstat(fullpath, &stat);
	if (ret < 0)
	{
		free(fullpath);
		errno = __vita_sce_errno_to_errno(ret, ERROR_GENERIC);
		return NULL;
	}

	if (!resolved_path)
	{
		resolved_path = fullpath;
	}
	else
	{
		strcpy(resolved_path, fullpath);
		free(fullpath);
	}

	return resolved_path;
}

int chdir(const char *path)
{
	char *fullpath = NULL;

	if (!path) // different error
	{
		errno = EFAULT;
		return -1;
	}

	fullpath = __realpath(path);
	if (!fullpath)
	{
		return -1; // errno already set
	}

	int ret = __is_dir(fullpath);
	if (ret < 0)
	{
		free(fullpath);
		errno = __vita_sce_errno_to_errno(ret, ERROR_GENERIC);
		return -1;
	}

	strcpy(__cwd, fullpath);
	free(fullpath);
	return 0;
}

int fchdir(int fd)
{
	int ret;
	DescriptorTranslation *fdmap = __vita_fd_grab(fd);

	if (!fdmap)
	{
		errno = EBADF;
		return -1;
	}

	switch (fdmap->type)
	{
		case VITA_DESCRIPTOR_TTY:
		case VITA_DESCRIPTOR_FILE:
		case VITA_DESCRIPTOR_SOCKET:
			__vita_fd_drop(fdmap);
			errno = ENOTDIR;
			return -1;
			break;
		case VITA_DESCRIPTOR_DIRECTORY:
			ret = chdir(fdmap->filename);
			__vita_fd_drop(fdmap);
			return ret;
	}
	return 0;
}
