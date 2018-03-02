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

#include <sys/dirent.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>

#include <psp2/types.h>
#include <psp2/io/dirent.h>
#include <psp2/kernel/threadmgr.h>

#define SCE_ERRNO_MASK 0xFF
#define MAXNAMLEN	256

struct DIR_
{
	SceUID uid;
	struct dirent dir;
	char dirname[MAXNAMLEN];
	int index;
};

int	closedir(DIR *dirp)
{
	if (!dirp || dirp->uid < 0)
	{
		errno = EBADF;
		return -1;
	}

	int res = sceIoDclose(dirp->uid);
	dirp->uid = -1;

	free(dirp);

	if (res < 0)
	{
		errno = res & SCE_ERRNO_MASK;
		return -1;
	}

	errno = 0;
	return 0;
}

DIR *opendir(const char *dirname)
{
	SceUID uid = sceIoDopen(dirname);

	if (uid < 0)
	{
		errno = uid & SCE_ERRNO_MASK;
		return NULL;
	}

	DIR *dirp = calloc(1, sizeof(DIR));

	if (!dirp)
	{
		sceIoDclose(uid);
		errno = ENOMEM;
		return NULL;
	}

	dirp->uid = uid;
	strncpy(dirp->dirname, dirname, sizeof(dirp->dirname)-1 );
	dirp->index = 0;

	errno = 0;
	return dirp;
}

struct dirent *readdir(DIR *dirp)
{
	if (!dirp)
	{
		errno = EBADF;
		return NULL;
	}

	int res = sceIoDread(dirp->uid, (SceIoDirent *)&dirp->dir);

	if (res < 0)
	{
		errno = res & SCE_ERRNO_MASK;
		return NULL;
	}

	if (res == 0)
	{
		errno = 0;
		return NULL;
	}

	struct dirent *dir = &dirp->dir;
	dirp->index++;
	return dir;
}

void rewinddir(DIR *dirp)
{
	if (!dirp)
	{
		errno = EBADF;
		return;
	}

	SceUID dirfd = sceIoDopen(dirp->dirname);

	if (dirfd < 0)
	{
		errno = dirfd & SCE_ERRNO_MASK;
		return;
	}

	sceIoDclose(dirp->uid);

	dirp->uid = dirfd;
	dirp->index = 0;
	errno = 0;
}

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

long int telldir(DIR *dirp)
{
	if (!dirp)
	{
		errno = EBADF;
		return -1;
	}

	return dirp->index;
}
