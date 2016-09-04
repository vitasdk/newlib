#include <errno.h>
#include <fcntl.h>
#include <reent.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/unistd.h>

#include <psp2/io/fcntl.h>
#include <psp2/io/dirent.h>
#include <psp2/io/stat.h>
#include <psp2/rtc.h>

#include <psp2/kernel/processmgr.h>

#include <string.h>

#include "vitadescriptor.h"
#include "vitaglue.h"


// TODO: add to SDK
int sceKernelLibcGettimeofday(struct timeval *ptimeval, void *ptimezone);

#define SCE_ERRNO_MASK 0xFF

_ssize_t
_write_r(struct _reent * reent, int fd, const void *buf, size_t nbytes)
{
	int ret;

	if (!is_fd_valid(fd)) {
		reent->_errno = EBADF;
		return -1;
	}

	switch (__vita_fdmap[fd]->type)
	{
	case VITA_DESCRIPTOR_FILE:
	case VITA_DESCRIPTOR_TTY:
		ret = sceIoWrite(__vita_fdmap[fd]->sce_uid, buf, nbytes);
		break;
	case VITA_DESCRIPTOR_SOCKET:
		if (__vita_glue_socket_send)
			ret = __vita_glue_socket_send(fd, buf, nbytes, 0);
		break;
	}

	if (ret < 0) {
		if (ret != -1)
			reent->_errno = ret & SCE_ERRNO_MASK;
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
	int ret = 0;

	if (!is_fd_valid(fd)) {
		reent->_errno = EBADF;
		return -1;
	}

	switch (__vita_fdmap[fd]->type)
	{
	case VITA_DESCRIPTOR_FILE:
	case VITA_DESCRIPTOR_TTY:
	{
		int sce_uid = __vita_fdmap[fd]->sce_uid;
		int ref_count = __vita_release_descriptor(fd);

		if (ref_count == 0)
			ret = sceIoClose(fd);
		break;
	}
	case VITA_DESCRIPTOR_SOCKET:
		if (__vita_glue_socket_close)
			ret = __vita_glue_socket_close(fd);
		break;
	}

	if (ret < 0) {
		if (ret != -1)
			reent->_errno = ret & SCE_ERRNO_MASK;
		return -1;
	}

	reent->_errno = 0;
	return ret;
}

char *__env[1] = { 0 };
char **environ = __env;

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
	int ret = sceKernelLibcGettimeofday(ptimeval, ptimezone);
	if (ret < 0) {
		reent->_errno = ret & SCE_ERRNO_MASK;
		return -1;
	}
	reent->_errno = 0;
	return 0;
}

int
_isatty_r(struct _reent *reent, int fd)
{
	if (!is_fd_valid(fd)) {
		reent->_errno = EBADF;
		return 0;
	}

	return (__vita_fdmap[fd]->type == VITA_DESCRIPTOR_TTY);
}

int
_kill_r(struct _reent *reent, int pid, int sig)
{
	if (pid != sceKernelGetProcessId()) {
		reent->_errno = EPERM;
		return -1;
	}
	switch (sig) {
	default:
		__builtin_trap();
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

	if (!is_fd_valid(fd)) {
		reent->_errno = EBADF;
		return -1;
	}

	switch (__vita_fdmap[fd]->type)
	{
	case VITA_DESCRIPTOR_FILE:
		ret = sceIoLseek32(__vita_fdmap[fd]->sce_uid, ptr, dir);
		break;
	case VITA_DESCRIPTOR_TTY:
	case VITA_DESCRIPTOR_SOCKET:
		ret = EBADF;
		break;
	}

	if (ret < 0) {
		reent->_errno = ret & SCE_ERRNO_MASK;
		return -1;
	}

	return ret;
}

int _fcntl2sony(int flags) {
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
	int ret, i, found = 0;
	flags = _fcntl2sony(flags);

	ret = sceIoOpen(file, flags, 0666);
	if (ret < 0) {
		reent->_errno = ret & SCE_ERRNO_MASK;
		return -1;
	}

	int fd = __vita_acquire_descriptor();

	if (fd < 0)
	{
		sceIoClose(ret);
		reent->_errno = EMFILE;
		return -1;
	}

	__vita_fdmap[fd]->sce_uid = ret;
	__vita_fdmap[fd]->type = VITA_DESCRIPTOR_FILE;

	reent->_errno = 0;
	return found;
}

_ssize_t
_read_r(struct _reent *reent, int fd, void *ptr, size_t len)
{
	int ret;

	if (!is_fd_valid(fd)) {
		reent->_errno = EBADF;
		return -1;
	}

	switch (__vita_fdmap[fd]->type)
	{
	case VITA_DESCRIPTOR_TTY:
	case VITA_DESCRIPTOR_FILE:
		ret = sceIoRead(__vita_fdmap[fd]->sce_uid, ptr, len);
		break;
	case VITA_DESCRIPTOR_SOCKET:
		if (__vita_glue_socket_recv)
			ret = __vita_glue_socket_recv(fd, ptr, len, 0);
		break;
	}

	if (ret < 0) {
		if (ret != -1)
			reent->_errno = ret & SCE_ERRNO_MASK;
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
	ret = sceIoRemove(path);
	if (ret < 0) {
		reent->_errno = ret & SCE_ERRNO_MASK;
		return -1;
	}
	reent->_errno = 0;
	return 0;
}

int
_rename_r(struct _reent *reent, const char *old, const char *new)
{
	int ret;
	ret = sceIoRename(old, new);
	if (ret < 0) {
		reent->_errno = ret & SCE_ERRNO_MASK;
		return -1;
	}
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
scestat_to_stat(struct SceIoStat *in, struct stat *out) {
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

	if (!is_fd_valid(fd)) {
		reent->_errno = EBADF;
		return -1;
	}

	switch (__vita_fdmap[fd]->type)
	{
	case VITA_DESCRIPTOR_TTY:
	case VITA_DESCRIPTOR_FILE:
		ret = sceIoGetstatByFd(__vita_fdmap[fd]->sce_uid, &stat);
		break;
	case VITA_DESCRIPTOR_SOCKET:
		ret = EBADF;
		break;
	}

	if (ret < 0) {
		reent->_errno = ret & SCE_ERRNO_MASK;
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
	if ((ret = sceIoGetstat(path, &stat)) < 0) {
		reent->_errno = ret & SCE_ERRNO_MASK;
		return -1;
	}
	scestat_to_stat(&stat, st);
	reent->_errno = 0;
	return 0;
}
