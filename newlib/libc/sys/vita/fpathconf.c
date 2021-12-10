/*

Copyright (c) 2021 vitasdk

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
#include <stddef.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>

/* Get file-specific information about descriptor FD.  */
long int
fpathconf(int fd, int name)
{
	if (fd < 0)
	{
		errno = EBADF;
		return -1;
	}

	switch (name)
	{
		default:
			errno = EINVAL;
			return -1;

		case _PC_LINK_MAX:
#ifdef LINK_MAX
			return LINK_MAX;
#else
			return -1;
#endif

		case _PC_MAX_CANON:
#ifdef MAX_CANON
			return MAX_CANON;
#else
			return -1;
#endif

		case _PC_MAX_INPUT:
#ifdef MAX_INPUT
			return MAX_INPUT;
#else
			return -1;
#endif

		case _PC_NAME_MAX:
#ifdef NAME_MAX
		return NAME_MAX;
#else
		return -1;
#endif

		case _PC_PATH_MAX:
#ifdef PATH_MAX
			return PATH_MAX;
#else
			return -1;
#endif

		case _PC_PIPE_BUF:
#ifdef  PIPE_BUF
			return PIPE_BUF;
#else
			return -1;
#endif

		case _PC_CHOWN_RESTRICTED:
#ifdef _POSIX_CHOWN_RESTRICTED
			return _POSIX_CHOWN_RESTRICTED;
#else
			return -1;
#endif

		case _PC_NO_TRUNC:
#ifdef _POSIX_NO_TRUNC
			return _POSIX_NO_TRUNC;
#else
			return -1;
#endif

		case _PC_VDISABLE:
#ifdef _POSIX_VDISABLE
			return _POSIX_VDISABLE;
#else
			return -1;
#endif

	case _PC_SYNC_IO:
#ifdef _POSIX_SYNC_IO
			return _POSIX_SYNC_IO;
#else
			return -1;
#endif

		case _PC_ASYNC_IO:
#ifdef _POSIX_ASYNC_IO
		{
			/* AIO is only allowed on regular files and block devices.  */
			struct stat st;

			if (fstat(fd, &st) < 0
				|| (! S_ISREG (st.st_mode) && ! S_ISBLK (st.st_mode)))
				return -1;
			else
				return 1;
		}
#else
		return -1;
#endif

		case _PC_PRIO_IO:
#ifdef _POSIX_PRIO_IO
			return _POSIX_PRIO_IO;
#else
			return -1;
#endif

		case _PC_FILESIZEBITS:
#ifdef FILESIZEBITS
			return FILESIZEBITS;
#else
			return 32;
#endif

		case _PC_REC_INCR_XFER_SIZE:
			return 64;

		case _PC_REC_MAX_XFER_SIZE:
			return -1;

		case _PC_REC_MIN_XFER_SIZE:
			return -1;

		case _PC_REC_XFER_ALIGN:
			return 64;

		case _PC_ALLOC_SIZE_MIN:
			return -1;

		case _PC_SYMLINK_MAX:
			return 0;
	}

}
