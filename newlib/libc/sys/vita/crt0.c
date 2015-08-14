int main(int argc, const char* argv[]);

void _init_vita_newlib(void) {
	_init_vita_heap();
	_init_vita_reent();
	_init_vita_malloc();
}

void _start() {
	_init_vita_newlib();
	exit(main(1, (const char*[]){"", 0}));
}
