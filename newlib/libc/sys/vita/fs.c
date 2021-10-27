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
#include <string.h>
#include <sys/dirent.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#include <psp2/types.h>
#include <psp2/io/stat.h>
#include <psp2/io/fcntl.h>

#include "vitadescriptor.h"

#define SCE_ERRNO_MASK 0xFF

int mkdir(const char *pathname, mode_t mode)
{
	return _mkdir_r(_REENT, pathname, mode);
}

int rmdir(const char *pathname)
{
	int ret;
	if ((ret = sceIoRmdir(pathname)) < 0) {
		errno = ret & SCE_ERRNO_MASK;
		return -1;
	}
	errno = 0;
	return 0;
}

int fsync (int __fd)
{
	int ret;

	DescriptorTranslation *fdmap = __vita_fd_grab(__fd);

	if (!fdmap) {
		errno = EBADF;
		return -1;
	}

	switch (fdmap->type)
	{
	case VITA_DESCRIPTOR_FILE:
		ret = sceIoSyncByFd(fdmap->sce_uid, 0);
		break;
	case VITA_DESCRIPTOR_TTY:
	case VITA_DESCRIPTOR_SOCKET:
	case VITA_DESCRIPTOR_DIR:
		__vita_fd_drop(fdmap);
		errno = EINVAL;
		return -1;
	}

	__vita_fd_drop(fdmap);

	if (ret < 0) {
		errno = ret & SCE_ERRNO_MASK;
		return -1;
	}

	errno = 0;
	return ret;
}
