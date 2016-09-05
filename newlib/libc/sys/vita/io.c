#include <fcntl.h>
#include <sys/unistd.h>
#include <errno.h>
#include <string.h>

#include <psp2/io/fcntl.h>
#include <psp2/kernel/threadmgr.h>

#include "vitadescriptor.h"
#include "vitaglue.h"

#define SCE_ERRNO_MASK 0xFF

DescriptorTranslation *__vita_fdmap[MAX_OPEN_FILES];
DescriptorTranslation __vita_fdmap_pool[MAX_OPEN_FILES];

SceKernelLwMutexWork _newlib_fd_mutex __attribute__ ((aligned (8)));

void _init_vita_io(void) {
	int ret;
	sceKernelCreateLwMutex(&_newlib_fd_mutex, "fd conversion table mutex", 0, 1, NULL);

	memset(__vita_fdmap, 0, sizeof(__vita_fdmap));
	memset(__vita_fdmap_pool, 0, sizeof(__vita_fdmap_pool));

	// TODO: do we prefer sceKernelStdin and friends?
	ret = sceIoOpen("tty0:", SCE_O_RDONLY, 0);

	if (ret >= 0) {
		__vita_fdmap[STDIN_FILENO] = &__vita_fdmap_pool[STDIN_FILENO];
		__vita_fdmap[STDIN_FILENO]->sce_uid = ret;
		__vita_fdmap[STDIN_FILENO]->type = VITA_DESCRIPTOR_TTY;
		__vita_fdmap[STDIN_FILENO]->ref_count = 1;
	}

	ret = sceIoOpen("tty0:", SCE_O_WRONLY, 0);

	if (ret >= 0) {
		__vita_fdmap[STDOUT_FILENO] = &__vita_fdmap_pool[STDOUT_FILENO];
		__vita_fdmap[STDOUT_FILENO]->sce_uid = ret;
		__vita_fdmap[STDOUT_FILENO]->type = VITA_DESCRIPTOR_TTY;
		__vita_fdmap[STDOUT_FILENO]->ref_count = 1;
	}

	ret = sceIoOpen("tty0:", SCE_O_WRONLY, 0);

	if (ret >= 0) {
		__vita_fdmap[STDERR_FILENO] = &__vita_fdmap_pool[STDERR_FILENO];
		__vita_fdmap[STDERR_FILENO]->sce_uid = ret;
		__vita_fdmap[STDERR_FILENO]->type = VITA_DESCRIPTOR_TTY;
		__vita_fdmap[STDERR_FILENO]->ref_count = 1;
	}

	sceKernelUnlockLwMutex(&_newlib_fd_mutex, 1);
}

void _free_vita_io(void) {
	sceKernelLockLwMutex(&_newlib_fd_mutex, 1, 0);

	if (__vita_fdmap[STDIN_FILENO]) {
		sceIoClose(__vita_fdmap[STDIN_FILENO]->sce_uid);
		memset(__vita_fdmap[STDIN_FILENO], 0, sizeof(DescriptorTranslation));
		__vita_fdmap[STDIN_FILENO] = NULL;
	}
	if (__vita_fdmap[STDOUT_FILENO]) {
		sceIoClose(__vita_fdmap[STDOUT_FILENO]->sce_uid);
		memset(__vita_fdmap[STDOUT_FILENO], 0, sizeof(DescriptorTranslation));
		__vita_fdmap[STDOUT_FILENO] = NULL;
	}
	if (__vita_fdmap[STDERR_FILENO]) {
		sceIoClose(__vita_fdmap[STDERR_FILENO]->sce_uid);
		memset(__vita_fdmap[STDERR_FILENO], 0, sizeof(DescriptorTranslation));
		__vita_fdmap[STDERR_FILENO] = NULL;
	}

	sceKernelUnlockLwMutex(&_newlib_fd_mutex, 1);
	sceKernelDeleteLwMutex(&_newlib_fd_mutex);
}

int __vita_acquire_descriptor(void)
{
	int fd = -1;
	int i = 0;
	sceKernelLockLwMutex(&_newlib_fd_mutex, 1, 0);

	// get free descriptor
	for (fd = 0; fd < MAX_OPEN_FILES; ++fd)
	{
		if (__vita_fdmap[fd] == NULL)
		{
			// get free pool
			for (i = 0; i < MAX_OPEN_FILES; ++i)
			{
				if (__vita_fdmap_pool[i].ref_count == 0)
				{
					__vita_fdmap[fd] = &__vita_fdmap_pool[i];
					__vita_fdmap[fd]->ref_count = 1;
					sceKernelUnlockLwMutex(&_newlib_fd_mutex, 1);
					return fd;
				}
			}
		}
	}

	// no mores descriptors available...
	sceKernelUnlockLwMutex(&_newlib_fd_mutex, 1);
	return -1;
}

int __vita_release_descriptor(int fd)
{
	DescriptorTranslation *map = NULL;

	sceKernelLockLwMutex(&_newlib_fd_mutex, 1, 0);

	if (!is_fd_valid(fd))
	{
		sceKernelUnlockLwMutex(&_newlib_fd_mutex, 1);
		return 0;
	}

	if (__vita_fd_drop(__vita_fdmap[fd]) < 0)
	{
		sceKernelUnlockLwMutex(&_newlib_fd_mutex, 1);
		return -1;
	}

	__vita_fdmap[fd] = NULL;

	sceKernelUnlockLwMutex(&_newlib_fd_mutex, 1);
	return 0;
}

int __vita_descriptor_ref_count(int fd)
{
	int res = 0;
	sceKernelLockLwMutex(&_newlib_fd_mutex, 1, 0);
	res = __vita_fdmap[fd]->ref_count;
	sceKernelUnlockLwMutex(&_newlib_fd_mutex, 1);
	return res;
}

DescriptorTranslation *__vita_fd_grab(int fd)
{
	DescriptorTranslation *map = NULL;

	sceKernelLockLwMutex(&_newlib_fd_mutex, 1, 0);

	if (!is_fd_valid(fd))
	{
		sceKernelUnlockLwMutex(&_newlib_fd_mutex, 1);
		return NULL;
	}

	map = __vita_fdmap[fd];

	if (map)
		map->ref_count++;

	sceKernelUnlockLwMutex(&_newlib_fd_mutex, 1);
	return map;
}

int __vita_fd_drop(DescriptorTranslation *map)
{
	sceKernelLockLwMutex(&_newlib_fd_mutex, 1, 0);

	if (map->ref_count == 1)
	{
		int ret = 0;

		switch (map->type)
		{
		case VITA_DESCRIPTOR_FILE:
		case VITA_DESCRIPTOR_TTY:
		{
			ret = sceIoClose(map->sce_uid);
			break;
		}
		case VITA_DESCRIPTOR_SOCKET:
			if (__vita_glue_socket_close)
				ret = __vita_glue_socket_close(map->sce_uid);
			break;
		}

		if (ret < 0) {
			sceKernelUnlockLwMutex(&_newlib_fd_mutex, 1);
			return -(ret & SCE_ERRNO_MASK);
		}

		map->ref_count--;
		memset(map, 0, sizeof(DescriptorTranslation));
	}
	else
	{
		map->ref_count--;
	}

	sceKernelUnlockLwMutex(&_newlib_fd_mutex, 1);
	return 0;
}
