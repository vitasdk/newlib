extern char _newlib_fd_mutex[32];

int main(int argc, const char* argv[]);

void _init_vita_newlib(void) {
	_init_vita_heap();
	_init_vita_reent();
	_init_vita_malloc();
	sceKernelCreateLwMutex(_newlib_fd_mutex, "fd conversion table mutex", 0, 0, 0);
}

void _start() {
	_init_vita_newlib();
	exit(main(1, (const char*[]){"", 0}));
}
