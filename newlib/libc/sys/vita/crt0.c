int main(int argc, const char* argv[]);

extern int _newlib_heap_memblock;
extern unsigned _newlib_heap_size;
extern char *_newlib_heap_base, *_newlib_heap_end, *_newlib_heap_cur;
extern char _newlib_sbrk_mutex[32];

void _init_vita_heap(void) {
	int ret;
	// Create a mutex to use inside _sbrk_r
	if ((ret = sceKernelCreateLwMutex(_newlib_sbrk_mutex, "sbrk mutex", 0, 0, 0)) < 0) {
		goto failure;
	}
	// Create a memblock for the heap memory, 32MB
	_newlib_heap_size = 32 * 1024 * 1024;
	_newlib_heap_memblock = sceKernelAllocMemBlock("Newlib heap", 0x0c20d060, _newlib_heap_size, 0);
	if (_newlib_heap_memblock < 0) {
		goto failure;
	}
	if (sceKernelGetMemBlockBase(_newlib_heap_memblock, &_newlib_heap_base) < 0) {
		goto failure;
	}
	_newlib_heap_end = _newlib_heap_base + _newlib_heap_size;
	_newlib_heap_cur = _newlib_heap_base;

	return;
failure:
	_newlib_heap_memblock = 0;
	_newlib_heap_base = 0;
	_newlib_heap_cur = 0;
}

void _init_vita_reent(void);

void _init_vita_newlib(void) {
	_init_vita_heap();
	_init_vita_reent();
}

void _start() {
	_init_vita_newlib();
	exit(main(1, (const char*[]){"", 0}));
}
