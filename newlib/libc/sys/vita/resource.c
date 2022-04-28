/*

Copyright (c) 2022 vitasdk

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

#include <errno.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <psp2/kernel/threadmgr.h>
#include "vitaglue.h"
#include "vitadescriptor.h"

int getpriority(int which, id_t who)
{
	// TODO: implement
	errno = ENOSYS;
	return -1;
}

int setpriority(int which, id_t who, int prio)
{
	// TODO: implement
	errno = ENOSYS;
	return -1;
}

int getrlimit(int resource, struct rlimit *rlim)
{
	if (rlim == NULL)
	{
		errno = EINVAL;
		return -1;
	}

	switch (resource)
	{
		case RLIMIT_CORE:
			rlim->rlim_cur = 0;
			rlim->rlim_max = 0;
			return 0;
		case RLIMIT_CPU:
			rlim->rlim_cur = RLIM_INFINITY;
			rlim->rlim_max = RLIM_INFINITY;
			return 0;
		case RLIMIT_DATA: // newlib heap size
		{
			unsigned int heap_size = _get_vita_heap_size();
			rlim->rlim_cur = heap_size;
			rlim->rlim_max = heap_size;
			return 0;
		}
		case RLIMIT_FSIZE: // Technically, 16 exabytes
			rlim->rlim_cur = RLIM_INFINITY;
			rlim->rlim_max = RLIM_INFINITY;
			return 0;
		case RLIMIT_NOFILE:
			rlim->rlim_cur = MAX_OPEN_FILES;
			rlim->rlim_max = MAX_OPEN_FILES;
			return 0;
		case RLIMIT_STACK: // get from thread
		{
			SceKernelThreadInfo info = {0};
			info.size = sizeof(SceKernelThreadInfo);
			sceKernelGetThreadInfo(0, &info);
			rlim->rlim_cur = info.stackSize;
			rlim->rlim_max = info.stackSize;
			return 0;
		}
		case RLIMIT_AS: // virtual address space
			rlim->rlim_cur = RLIM_INFINITY;
			rlim->rlim_max = RLIM_INFINITY;
			return 0;
		default:
			errno = EINVAL;
			return -1;
	}
}

int setrlimit(int resource, const struct rlimit *rlim)
{
	// TODO: implement or return EINVAL?
	errno = ENOSYS;
	return -1;
}

int getrusage(int who, struct rusage *usage)
{
	if (usage == NULL)
	{
		errno = EINVAL;
		return -1;
	}

	if (who = RUSAGE_SELF)
	{
		SceKernelThreadInfo info = {0};
		info.size = sizeof(SceKernelThreadInfo);
		sceKernelGetThreadInfo(0, &info);

		usage->ru_utime.tv_sec = info.runClocks / 1000000;
		usage->ru_utime.tv_usec = info.runClocks - (info.runClocks / 1000000);
		usage->ru_stime.tv_sec = 0;
		usage->ru_stime.tv_usec = 0;
	}
	errno = EINVAL;
	return -1;
}
