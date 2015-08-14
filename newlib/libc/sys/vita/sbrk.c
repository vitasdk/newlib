#include <errno.h>
#include <reent.h>

int _newlib_heap_memblock;
unsigned _newlib_heap_size;
char *_newlib_heap_base, *_newlib_heap_end, *_newlib_heap_cur;

char _newlib_sbrk_mutex[32] __attribute__ ((aligned (8)));

void * _sbrk_r(struct _reent *reent, ptrdiff_t incr) {
	if (sceKernelLockLwMutex(_newlib_sbrk_mutex, 1, 0) < 0)
		goto fail;
	if (!_newlib_heap_base || _newlib_heap_cur + incr >= _newlib_heap_end) {
		sceKernelUnlockLwMutex(_newlib_sbrk_mutex, 1);
fail:
		reent->_errno = ENOMEM;
		return -1;
	}

	char *prev_heap_end = _newlib_heap_cur;
	_newlib_heap_cur += incr;

	sceKernelUnlockLwMutex(_newlib_sbrk_mutex, 1);
	return (void*) prev_heap_end;
}
