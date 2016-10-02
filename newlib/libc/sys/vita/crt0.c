/* The maximum number of arguments that can be passed to main(). */
#define ARGC_MAX 19

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

/* 
 * Code below is based on the PSPSDK implementation  
 * Copyright (c) 2005 Marcus R. Brown <mrbrown@ocgnet.org> 
 */

void _start(unsigned int args, void *argp)
{
	char *argv[ARGC_MAX + 1] = {""}; // Program name
	int argc = 1;
	int loc = 0;
	char *ptr = argp;

	/* Turn our thread arguments into main()'s argc and argv[]. */
	while(loc < args)
	{
		argv[argc] = &ptr[loc];
		loc += strlen(&ptr[loc]) + 1;
		argc++;
		if(argc == ARGC_MAX)
		{
			break;
		}
	}

	argv[argc] = 0;
	_init_vita_newlib();
	__libc_init_array();
	exit(main(argc, argv));
}
