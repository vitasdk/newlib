/*

Copyright (c) 2023 vitasdk

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

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>

#include <psp2/net/net.h>
#include <psp2/types.h>

#include "vitadescriptor.h"
#include "vitaerror.h"
#include "vitanet.h"

extern FILE  *__sfp (struct _reent *);
#define SETFL_FLAGS (O_NONBLOCK | O_APPEND | O_DIRECT | O_ASYNC | O_SYNC | O_DSYNC)

int _fcntl_r(struct _reent *reent, int fd, int cmd, ...)
{
	int arg = 0;
	if ((cmd == F_DUPFD) || (cmd == F_SETFD) || (cmd == F_SETFL))
	{
		va_list args;
		va_start(args, cmd);
		arg = va_arg(args, int);
		va_end(args);
	}

	if (cmd == F_DUPFD) {
		if (arg < 0 || arg >= MAX_OPEN_FILES) {
			errno = EINVAL;
			return -1;
		}

		int dupfd = __vita_duplicate_descriptor(fd, arg);
		if (dupfd < 0)
		{
			errno = EBADF;
			return -1;
		}

		errno = 0;
		return dupfd;
	}

	DescriptorTranslation *fdmap = __vita_fd_grab(fd);
	if (fdmap == NULL)
	{
		errno = EBADF;
		return -1;
	}

	// The only existing flag is FD_CLOEXEC, and it is unsupported,
	// so F_GETFD/F_SETFD always returns zero.
	if (cmd == F_GETFD || cmd == F_SETFD)
	{
		__vita_fd_drop(fdmap);
		return 0;
	}

	if (fdmap->type == VITA_DESCRIPTOR_SOCKET)
	{
		if (cmd == F_GETFL)
		{
			int optval = 0;
			socklen_t optlen = sizeof(optval);
			int res = sceNetGetsockopt(fdmap->sce_uid, SOL_SOCKET, SO_NONBLOCK, &optval, &optlen);

			__vita_fd_drop(fdmap);
			if (res < 0)
			{
				errno = __vita_scenet_errno_to_errno(res);
				return -1;
			}

			return (optval) ? O_NONBLOCK : 0;
		}

		if (cmd == F_SETFL)
		{
			int optval = (arg & O_NONBLOCK) ? 1 : 0;
			socklen_t optlen = sizeof(optval);

			int optres = sceNetSetsockopt(fdmap->sce_uid, SOL_SOCKET, SO_NONBLOCK, &optval, optlen);

			__vita_fd_drop(fdmap);
			if (optres < 0)
			{
				errno = __vita_scenet_errno_to_errno(optres);
				return -1;
			}

			return 0;
		}
	}

	if (fdmap->type == VITA_DESCRIPTOR_FILE)
	{
		if (cmd == F_GETFL)
		{
			int flags = fdmap->flags;
			__vita_fd_drop(fdmap);
			return flags;
		}

		if (cmd == F_SETFL)
		{
			// File access modes should be ignored
			arg = arg & SETFL_FLAGS;
			// But should be kept as is if they were set by open
			arg |= (fdmap->flags & ~SETFL_FLAGS);

			fdmap->flags = arg;
			__vita_fd_drop(fdmap);
			// update native descriptor flags too
			register FILE *fp;
			if ((fp = __sfp (reent)) == 0)
			{
				errno = EINVAL;
				return -1;
			}
			fp->_flags = arg;
			return 0;
		}
	}

	__vita_fd_drop(fdmap);
	errno = EBADF;
	return -1;
}
