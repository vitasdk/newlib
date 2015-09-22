static char _newlib_malloc_mutex[32] __attribute__ ((aligned (8)));

void __malloc_lock(struct _reent *r) {
	sceKernelLockLwMutex(_newlib_malloc_mutex, 1, 0);
}

void __malloc_unlock(struct _reent *r) {
	sceKernelUnlockLwMutex(_newlib_malloc_mutex, 1);
}

void _init_vita_malloc(void) {
	sceKernelCreateLwMutex(_newlib_malloc_mutex, "malloc mutex", 2, 0, 0);
}
