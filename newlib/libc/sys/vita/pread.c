#include <errno.h>
#include <sys/types.h>
#include <sys/unistd.h>

#include <psp2/io/fcntl.h>

#include "vitadescriptor.h"
#include "vitaerror.h"

#define SCE_ERRNO_MASK 0xFF

ssize_t pread (int __fd, void *__buf, size_t __nbytes, off_t __offset)
{
	int ret;
	DescriptorTranslation *fdmap = __vita_fd_grab(__fd);

	if (!fdmap) {
		errno = EBADF;
		return -1;
	}

	ret = sceIoPread(fdmap->sce_uid, __buf, __nbytes, __offset);

	__vita_fd_drop(fdmap);

	if (ret < 0) {
		if (ret != -1 )
			errno = ret & SCE_ERRNO_MASK;
		return -1;
	}

	errno = 0;
	return ret;
}

ssize_t pwrite (int __fd, const void *__buf, size_t __nbytes, off_t __offset)
{
	int ret;
	DescriptorTranslation *fdmap = __vita_fd_grab(__fd);

	if (!fdmap) {
		errno = EBADF;
		return -1;
	}

	ret = sceIoPwrite(fdmap->sce_uid, __buf, __nbytes, __offset);

	__vita_fd_drop(fdmap);

	if (ret < 0) {
		if (ret != -1)
			errno = ret & SCE_ERRNO_MASK;
		return -1;
	}

	errno = 0;
	return ret;
}
