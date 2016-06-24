int main(int argc, const char* argv[]);

void _init_vita_newlib(void) {
	_init_vita_heap();
	_init_vita_reent();
	_init_vita_malloc();
	_init_vita_io();
}

void _free_vita_newlib(void) {
	_free_vita_io();
	_free_vita_malloc();
	_free_vita_reent();
	_free_vita_heap();
}

void _start() {
	_init_vita_newlib();
	__libc_init_array();
	exit(main(1, (const char*[]){"", 0}));
}
