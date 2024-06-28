/*

Copyright (C) 2016, David "Davee" Morgan

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

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <psp2/types.h>
#include <psp2/io/dirent.h>
#include <psp2/kernel/threadmgr.h>
#include "vitadescriptor.h"
#include "vitaerror.h"

struct DIR_
{
	int fd;
	struct dirent dir;
	int index;
};

#ifdef F_closedir
int	closedir(DIR *dirp)
{
	if (!dirp || !dirp->fd)
	{
		errno = EBADF;
		return -1;
	}

	int res = close(dirp->fd);
	free(dirp);

	if (res < 0)
	{
		return -1;
	}

	errno = 0;
	return 0;
}
#endif

#ifdef F_opendir
DIR *__opendir_common(int fd)
{
	DIR *dirp = calloc(1, sizeof(DIR));

	if (!dirp)
	{
		errno = ENOMEM;
		return NULL;
	}

	dirp->fd = fd;
	dirp->index = 0;

	errno = 0;
	return dirp;
}

DIR *opendir(const char *dirname)
{
	int fd;

	if ((fd = open(dirname, O_RDONLY | O_DIRECTORY)) == -1)
		return (NULL);
	DIR *ret = __opendir_common(fd);
	if (!ret)
	{
		close(fd);
	}
	return ret;
}

DIR *fdopendir(int fd)
{
	DescriptorTranslation *fdmap = __vita_fd_grab(fd);
	if (!fdmap)
	{
		errno = EBADF;
		return NULL;
	}

	if (fdmap->type != VITA_DESCRIPTOR_DIRECTORY)
	{
		__vita_fd_drop(fdmap);
		errno = ENOTDIR;
		return NULL;
	}

	__vita_fd_drop(fdmap);

	return (__opendir_common(fd));
}
#endif

#ifdef F_readdir
struct dirent *readdir(DIR *dirp)
{
	if (!dirp)
	{
		errno = EBADF;
		return NULL;
	}

	DescriptorTranslation *fdmap = __vita_fd_grab(dirp->fd);
	if (!fdmap)
	{
		errno = EBADF;
		return NULL;
	}

	int res = sceIoDread(fdmap->sce_uid, (SceIoDirent *)&dirp->dir);

	__vita_fd_drop(fdmap);

	if (res < 0)
	{
		errno = __vita_sce_errno_to_errno(res, ERROR_GENERIC);
		return NULL;
	}

	if (res == 0)
	{
		// end-of-listing shouldn't change errno
		return NULL;
	}

	struct dirent *dir = &dirp->dir;
	dirp->index++;
	return dir;
}
#endif

#ifdef F_rewinddir
void rewinddir(DIR *dirp)
{
	if (!dirp)
	{
		errno = EBADF;
		return;
	}

	DescriptorTranslation *fdmap = __vita_fd_grab(dirp->fd);
	if (!fdmap)
	{
		errno = EBADF;
		return;
	}

	SceUID dirfd = sceIoDopen(fdmap->filename); // filename contains full path, so it's okay to use in sce funcs

	if (dirfd < 0)
	{
		__vita_fd_drop(fdmap);
		errno = __vita_sce_errno_to_errno(dirfd, ERROR_GENERIC);
		return;
	}

	sceIoDclose(fdmap->sce_uid);
	fdmap->sce_uid = dirfd;
	__vita_fd_drop(fdmap);

	dirp->index = 0;
	errno = 0;
}
#endif

#ifdef F_seekdir
void seekdir(DIR *dirp, long int index)
{
	if (!dirp)
	{
		errno = EBADF;
		return;
	}

	if (index < dirp->index)
		rewinddir(dirp);

	if (index < dirp->index)
	{
		return;
	}

	while (index != dirp->index)
	{
		if (!readdir(dirp))
		{
			errno = ENOENT;
			return;
		}
	}
}
#endif

#ifdef F_telldir
long int telldir(DIR *dirp)
{
	if (!dirp)
	{
		errno = EBADF;
		return -1;
	}

	return dirp->index;
}
#endif

#ifdef F_dirfd
int dirfd(DIR *dirp)
{
	if (!dirp)
	{
		errno = EINVAL;
		return -1;
	}

	return dirp->fd;
}
#endif

#ifdef F_readdir_r
int readdir_r(DIR *restrict dirp, struct dirent *restrict entry, struct dirent **restrict result)
{
	*result = NULL;
	errno = 0;
	struct dirent *next = readdir(dirp);
	if(errno != 0 && next == NULL)
	{
		return errno;
	}
	if(next)
	{
		memcpy(entry, next, sizeof(struct dirent));
		*result = entry;
	}
	return 0;
}
#endif

#ifdef F_scandir
int
alphasort(const struct dirent **d1, const struct dirent **d2)
{
	return (strcoll((*d1)->d_name, (*d2)->d_name));
}

// modified from musl
int scandir(const char *path, struct dirent ***res,
	int (*sel)(const struct dirent *),
	int (*cmp)(const struct dirent **, const struct dirent **))
{
	DIR *d = opendir(path);
	struct dirent *de, **names=0, **tmp;
	size_t cnt=0, len=0;
	int old_errno = errno;

	if (!d) return -1;

	while ((errno=0), (de = readdir(d)))
	{
		if (sel && !sel(de)) continue;
		if (cnt >= len)
		{
			len = 2*len+1;
			if (len > SIZE_MAX/sizeof *names) break;
			tmp = realloc(names, len * sizeof *names);
			if (!tmp) break;
			names = tmp;
		}
		names[cnt] = malloc(sizeof(struct dirent));
		if (!names[cnt]) break;
		memcpy(names[cnt++], de, sizeof(struct dirent));
	}

	closedir(d);

	if (errno)
	{
		if (names) while (cnt-- > 0) free(names[cnt]);
		free(names);
		return -1;
	}
	errno = old_errno;

	if (cmp) qsort(names, cnt, sizeof *names, (int (*)(const void *, const void *))cmp);
	*res = names;
	return cnt;
}
#endif
