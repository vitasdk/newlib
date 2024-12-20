#include <errno.h>
#include <fcntl.h>
#include <reent.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/unistd.h>

#include <psp2/io/fcntl.h>
#include <psp2/io/dirent.h>
#include <psp2/io/stat.h>
#include <psp2/rtc.h>
#include <psp2/net/net.h>

#include <psp2/kernel/processmgr.h>

#include <string.h>

#include "vitadescriptor.h"
#include "vitaglue.h"
#include "vitaerror.h"
#include "vitafs.h"

_ssize_t
_write_r(struct _reent * reent, int fd, const void *buf, size_t nbytes)
{
	int ret = 0;
	int type = ERROR_GENERIC;

	DescriptorTranslation *fdmap = __vita_fd_grab(fd);

	if (!fdmap) {
		reent->_errno = EBADF;
		return -1;
	}

	switch (fdmap->type)
	{
		case VITA_DESCRIPTOR_FILE:
		case VITA_DESCRIPTOR_TTY:
			if (fdmap->flags & O_APPEND)
				sceIoLseek(fdmap->sce_uid, 0, SCE_SEEK_END);
			ret = sceIoWrite(fdmap->sce_uid, buf, nbytes);
			break;
		case VITA_DESCRIPTOR_SOCKET:
			type = ERROR_SOCKET;
			ret = sceNetSend(fdmap->sce_uid, buf, nbytes, 0);
			break;
		case VITA_DESCRIPTOR_DIRECTORY:
			ret = __vita_make_sce_errno(EBADF);
			break;
		case VITA_DESCRIPTOR_PIPE:
		{
			size_t len = nbytes;
			if (len > 4 * 4096) len = 4 * 4096;
			size_t p_res = 0;
			ret = sceKernelSendMsgPipe(fdmap->sce_uid, buf, len, 1, &p_res, NULL);
			if (ret == 0) ret = p_res;
			break;
		}
	}

	__vita_fd_drop(fdmap);

	if (ret < 0) {
		if (ret != -1)
			reent->_errno = __vita_sce_errno_to_errno(ret, type);
		return -1;
	}

	reent->_errno = 0;
	return ret;
}

void
_exit(int rc)
{
	_free_vita_newlib();
	sceKernelExitProcess(rc);
	while (1);
}

int
_close_r(struct _reent *reent, int fd)
{
	int res = __vita_release_descriptor(fd);

	if (res < 0)
	{
		reent->_errno = EBADF;
		return -1;
	}

	reent->_errno = 0;
	return 0;
}

int
_execve_r(struct _reent *reent, const char *name, char * const *argv,
		char * const *env)
{
	reent->_errno = ENOSYS;
	return -1;
}

int
_fork_r(struct _reent *reent)
{
	reent->_errno = ENOSYS;
	return -1;
}

int
_getpid_r(struct _reent *reent)
{
	reent->_errno = 0;
	return sceKernelGetProcessId();
}

int
_gettimeofday_r(struct _reent *reent, struct timeval *ptimeval, void *ptimezone)
{
	int ret = sceKernelLibcGettimeofday((SceKernelTimeval*)ptimeval, (SceKernelTimezone *)ptimezone);
	if (ret < 0) {
		reent->_errno = __vita_sce_errno_to_errno(ret, ERROR_GENERIC);
		return -1;
	}
	reent->_errno = 0;
	return 0;
}

int
_isatty_r(struct _reent *reent, int fd)
{
	DescriptorTranslation *fdmap = __vita_fd_grab(fd);

	if (!fdmap) {
		reent->_errno = EBADF;
		return 0;
	}

	int istty = (fdmap->type == VITA_DESCRIPTOR_TTY);

	__vita_fd_drop(fdmap);
	return istty;
}

int
_kill_r(struct _reent *reent, int pid, int sig)
{
	if (pid != sceKernelGetProcessId())
	{
		reent->_errno = EPERM;
		return -1;
	}
	switch (sig)
	{
		default:
			__builtin_trap();
			break;
		case SIGINT:
		case SIGTERM:
			sceKernelExitProcess(-sig);
			break;
		case SIGCHLD:
		case SIGCONT:
			return 0;
	}
}

int
_link_r(struct _reent *reent, const char *existing, const char *new)
{
	reent->_errno = ENOSYS;
	return -1;
}

_off_t
_lseek_r(struct _reent *reent, int fd, _off_t ptr, int dir)
{
	int ret;
	DescriptorTranslation *fdmap = __vita_fd_grab(fd);

	switch (fdmap->type)
	{
		case VITA_DESCRIPTOR_FILE:
			ret = sceIoLseek32(fdmap->sce_uid, ptr, dir);
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

	return ret;
}

int 
_mkdir_r (struct _reent * reent, const char * path, int mode)
{
	int ret;
	char* full_path = __realpath(path);
	if(!full_path)
	{
		reent->_errno = errno; // set by realpath
		return -1;
	}
	if ((ret = sceIoMkdir(full_path, 0777)) < 0)
	{
		free(full_path);
		reent->_errno = __vita_sce_errno_to_errno(ret, ERROR_GENERIC);
		return -1;
	}
	free(full_path);
	reent->_errno = 0;
	return 0;
}

int _fcntl2sony(int flags)
{
	int out = 0;
	if (flags & O_RDWR)
		out |= SCE_O_RDWR;
	else if (flags & O_WRONLY)
		out |= SCE_O_WRONLY;
	else
		out |= SCE_O_RDONLY;
	if (flags & O_NONBLOCK)
		out |= SCE_O_NBLOCK;
	if (flags & O_APPEND)
		out |= SCE_O_APPEND;
	if (flags & O_CREAT)
		out |= SCE_O_CREAT;
	if (flags & O_TRUNC)
		out |= SCE_O_TRUNC;
	if (flags & O_EXCL)
		out |= SCE_O_EXCL;
	return out;
}

int
_open_r(struct _reent *reent, const char *file, int flags, int mode)
{
	int ret;
	int sce_flags = _fcntl2sony(flags);
	int is_dir = 0;

    // check flags

	char* full_path = __realpath(file);
	if (!full_path)
	{
		reent->_errno = errno; // set by realpath
		return -1;
	}

	// get full path and stat. if dir - use dir funcs, otherwise - file
	// if O_DIRECTORY passed - check that path is indeed dir and return ENOTDIR otherwise
	// ENOENT, etc is handled by sce funcs
	if (__is_dir(full_path) == 0) {
		is_dir = 1;
	}
	if (flags & O_DIRECTORY && !is_dir)
	{
		free(full_path);
		reent->_errno = ENOTDIR;
		return -1;
	}

	ret = is_dir ? sceIoDopen(full_path) : sceIoOpen(full_path, sce_flags, 0666);
	if (ret < 0)
	{
		free(full_path);
		reent->_errno = __vita_sce_errno_to_errno(ret, ERROR_GENERIC);
		return -1;
	}


	int fd = __vita_acquire_descriptor();

	if (fd < 0)
	{
		free(full_path);
		is_dir ? sceIoDclose(ret) : sceIoClose(ret);
		reent->_errno = EMFILE;
		return -1;
	}

	__vita_fdmap[fd]->sce_uid = ret;
	__vita_fdmap[fd]->type = is_dir ? VITA_DESCRIPTOR_DIRECTORY : VITA_DESCRIPTOR_FILE;
	__vita_fdmap[fd]->filename = strdup(full_path);
	__vita_fdmap[fd]->flags = flags;

	free(full_path);

	reent->_errno = 0;
	return fd;
}

_ssize_t
_read_r(struct _reent *reent, int fd, void *ptr, size_t len)
{
	int ret;
	int type = ERROR_GENERIC;
	DescriptorTranslation *fdmap = __vita_fd_grab(fd);

	if (!fdmap)
	{
		reent->_errno = EBADF;
		return -1;
	}

	switch (fdmap->type)
	{
		case VITA_DESCRIPTOR_TTY:
		case VITA_DESCRIPTOR_FILE:
			ret = sceIoRead(fdmap->sce_uid, ptr, len);
			break;
		case VITA_DESCRIPTOR_SOCKET:
			type = ERROR_SOCKET;
			ret = sceNetRecv(fdmap->sce_uid, ptr, len, 0);
			break;
		case VITA_DESCRIPTOR_DIRECTORY:
			ret = __vita_make_sce_errno(EBADF);
			break;
		case VITA_DESCRIPTOR_PIPE:
		{
			size_t rlen = len;
			if (rlen > 4 * 4096) rlen = 4 * 4096;
			size_t p_res = 0;
			ret = sceKernelReceiveMsgPipe(fdmap->sce_uid, ptr, rlen, 1, &p_res, NULL);
			if (ret == 0) ret = p_res;
			break;
		}
	}

	__vita_fd_drop(fdmap);

	if (ret < 0)
	{
		if (ret != -1)
			reent->_errno = __vita_sce_errno_to_errno(ret, type);
		return -1;
	}

	reent->_errno = 0;
	return ret;
}

int
_readlink_r(struct _reent *reent, const char *path, char *buf, size_t bufsize)
{
	reent->_errno = ENOSYS;
	return -1;
}

int
_unlink_r(struct _reent *reent, const char * path)
{
	int ret;
	char* full_path = __realpath(path);
	if (!full_path)
	{
		reent->_errno = errno; // set by realpath
		return -1;
	}
	ret = sceIoRemove(full_path);
	if (ret < 0)
	{
		free(full_path);
		reent->_errno = __vita_sce_errno_to_errno(ret, ERROR_GENERIC);
		return -1;
	}
	free(full_path);
	reent->_errno = 0;
	return 0;
}

int
_rename_r(struct _reent *reent, const char *old, const char *new)
{
	int ret;
	char* full_path_old = __realpath(old);
	if (!full_path_old)
	{
		reent->_errno = errno; // set by realpath
		return -1;
	}
	char* full_path_new = __realpath(new);
	if (!full_path_new)
	{
		free(full_path_old);
		reent->_errno = errno; // set by realpath
		return -1;
	}

	// NOOP
	if (strcmp(full_path_old, full_path_new) == 0)
	{
		free(full_path_old);
		free(full_path_new);
		reent->_errno = 0;
		return 0;
	}

	// sceIoRename fails if `new` exists.
	// But to conform with POSIX/BSD, we should remove `new` if it's file or empty dir
	ret = sceIoRemove(full_path_new);
	if (ret == 0x80010015) // EISDIR
	{
		// sceIoRmdir won't remove non-empty directory
		// we don't care about return, since sceIoRename will return proper error code (EEXIST) in this case
		sceIoRmdir(full_path_new);
	}

	ret = sceIoRename(full_path_old, full_path_new);
	if (ret < 0)
	{
		free(full_path_old);
		free(full_path_new);
		reent->_errno = __vita_sce_errno_to_errno(ret, ERROR_GENERIC);
		return -1;
	}
	free(full_path_old);
	free(full_path_new);
	reent->_errno = 0;
	return 0;
}

clock_t
_times_r(struct _reent *reent, struct tms *ptms)
{
	unsigned result = sceKernelGetProcessTimeLow();
	ptms->tms_utime = result;
	ptms->tms_stime = 0;
	ptms->tms_cutime = 0;
	ptms->tms_cstime = 0;
	return result;
}

static void
scestat_to_stat(struct SceIoStat *in, struct stat *out)
{
	memset(out, 0, sizeof(*out));
	out->st_size = in->st_size;
	if (SCE_S_ISREG(in->st_mode))
		out->st_mode |= _IFREG;
	if (SCE_S_ISDIR(in->st_mode))
		out->st_mode |= _IFDIR;
	sceRtcGetTime_t(&in->st_atime, &out->st_atime);
	sceRtcGetTime_t(&in->st_mtime, &out->st_mtime);
	sceRtcGetTime_t(&in->st_ctime, &out->st_ctime);
}

int
_fstat_r(struct _reent *reent, int fd, struct stat *st)
{
	struct SceIoStat stat = {0};
	int ret;

	DescriptorTranslation *fdmap = __vita_fd_grab(fd);

	if (!fdmap)
	{
		reent->_errno = EBADF;
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

	scestat_to_stat(&stat, st);
	reent->_errno = 0;
	return 0;
}

int
_stat_r(struct _reent *reent, const char *path, struct stat *st)
{
	struct SceIoStat stat = {0};
	int ret;
	char* full_path = __realpath(path);
	if (!full_path)
	{
		reent->_errno = errno; // set by realpath
		return -1;
	}
	if ((ret = sceIoGetstat(full_path, &stat)) < 0)
	{
		free(full_path);
		reent->_errno = __vita_sce_errno_to_errno(ret, ERROR_GENERIC);
		return -1;
	}
	free(full_path);
	scestat_to_stat(&stat, st);
	reent->_errno = 0;
	return 0;
}
