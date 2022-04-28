/*

Copyright (C) 2022, vitasdk

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>

long int sysconf(int name)
{
	switch (name)
	{
		// limits
		case _SC_ARG_MAX:
			return ARG_MAX;
		case _SC_CHILD_MAX:
			return CHILD_MAX;
		case _SC_CLK_TCK:
			return CLK_TCK;
		case _SC_NGROUPS_MAX:
			return NGROUPS_MAX;
		case _SC_OPEN_MAX:
			return OPEN_MAX;

		// hardcoded values
		case _SC_SEM_NSEMS_MAX:
			return 256;
		case _SC_SEM_VALUE_MAX:
			return 32767;
		case _SC_THREAD_KEYS_MAX:
			return 128;
		case _SC_THREAD_STACK_MIN:
			return 4096;
		case _SC_THREAD_THREADS_MAX:
			return 128;
		case _SC_SEMAPHORES:
			return 1;
		case _SC_THREADS:
			return 1;
		case _SC_THREAD_ATTR_STACKSIZE:
			return 1;
		case _SC_THREAD_SAFE_FUNCTIONS:
			return 1;
		case _SC_VERSION:
			return 200809L; // we (try to) target posix-2008
		default:
			errno = EINVAL;
			return -1;
	}

	return -1;
}
