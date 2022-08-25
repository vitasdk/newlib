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
#include <sys/types.h>
#include "fios2.h"

#include "vitadescriptor.h"
#include "vitaerror.h"

int truncate(const char *path, off_t length)
{
    struct _reent *reent = _REENT;
	int ret;

	ret = sceFiosFileTruncateSync(NULL, path, length);

	if (ret < 0)
	{
		reent->_errno = __vita_sce_errno_to_errno(ret, ERROR_GENERIC);
		return -1;
	}

	reent->_errno = 0;
	return 0;
}

int ftruncate(int fd, off_t length)
{
    struct _reent *reent = _REENT;
	int ret;
	
	DescriptorTranslation *fdmap = __vita_fd_grab(fd);

	if (!fdmap)
	{
		reent->_errno = EBADF;
		return -1;
	}

	switch (fdmap->type)
	{
		case VITA_DESCRIPTOR_FILE:
			ret = sceFiosFHTruncateSync(NULL, fdmap->sce_uid, length);
			break;
		case VITA_DESCRIPTOR_TTY:
		case VITA_DESCRIPTOR_SOCKET:
		case VITA_DESCRIPTOR_DIRECTORY:
		case VITA_DESCRIPTOR_PIPE:
			ret = __vita_make_sce_errno(EBADF);
			break;
	}

	__vita_fd_drop(fdmap);

	if (ret < 0)
	{
		reent->_errno = __vita_sce_errno_to_errno(ret, ERROR_GENERIC);
		return -1;
	}

	reent->_errno = 0;
	return 0;
}
