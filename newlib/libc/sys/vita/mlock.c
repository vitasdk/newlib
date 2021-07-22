#include <reent.h>
#include <psp2/kernel/threadmgr.h>

static SceKernelLwMutexWork _newlib_malloc_mutex;

void __malloc_lock(struct _reent *r) {
	sceKernelLockLwMutex(&_newlib_malloc_mutex, 1, 0);
}

void __malloc_unlock(struct _reent *r) {
	sceKernelUnlockLwMutex(&_newlib_malloc_mutex, 1);
}

void _init_vita_malloc(void) {
	sceKernelCreateLwMutex(&_newlib_malloc_mutex, "malloc mutex", 2, 0, 0);
}

void _free_vita_malloc(void) {
	sceKernelDeleteLwMutex(&_newlib_malloc_mutex);
}
