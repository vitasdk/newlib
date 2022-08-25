#include <errno.h>
#include <sys/types.h>
#include <sys/unistd.h>

#include <psp2/io/fcntl.h>
#include "fios2.h"

#include "vitadescriptor.h"
#include "vitaerror.h"

ssize_t pread (int __fd, void *__buf, size_t __nbytes, off_t __offset)
{
	int ret;
	DescriptorTranslation *fdmap = __vita_fd_grab(__fd);

	if (!fdmap) {
		errno = EBADF;
		return -1;
	}

	ret = sceFiosFHPreadSync(NULL, fdmap->sce_uid, __buf, __nbytes, __offset);

	__vita_fd_drop(fdmap);

	if (ret < 0) {
		if (ret != -1 )
			errno = __vita_sce_errno_to_errno(ret, ERROR_GENERIC);
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

	ret = sceFiosFHPwriteSync(fdmap->sce_uid, __buf, __nbytes, __offset);

	__vita_fd_drop(fdmap);

	if (ret < 0) {
		if (ret != -1)
			errno = __vita_sce_errno_to_errno(ret, ERROR_GENERIC);
		return -1;
	}

	errno = 0;
	return ret;
}
