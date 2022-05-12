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

#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/sysmodule.h>
#include <psp2/types.h>
#include <stdlib.h>

#include "vitanet.h"

// theoretical maximum:
// 128 bytes per socket, 1024 sockets max
// 4KiB reserved
// 3KiB - basic features
// 4KiB - dns cache
// 1300 bytes - dns resolver
#define NET_MEM_SIZE (128*1024 + ((4+3+4) * 1024)) + 1300

#ifndef SCE_NET_CTL_ERROR_NOT_TERMINATED
#define SCE_NET_CTL_ERROR_NOT_TERMINATED 0x80412102
#endif

static void *_vita_net_memory = NULL;
static int _vita_net_initialized = 0;

int _vita_net_init()
{
	int ret;

	if (!__sync_bool_compare_and_swap(&_vita_net_initialized, 0, 1))  return 0;

	SceNetInitParam net_init_param = {0};

	if (sceSysmoduleIsLoaded(SCE_SYSMODULE_NET) != SCE_SYSMODULE_LOADED)
	{
		ret = sceSysmoduleLoadModule(SCE_SYSMODULE_NET);
		if (ret < 0)
		{
			return ret;
		}
	}

	_vita_net_memory = malloc(NET_MEM_SIZE);
	if (!_vita_net_memory)
	{
		return SCE_NET_ENOMEM;
	}

	net_init_param.memory = _vita_net_memory;
	net_init_param.size = NET_MEM_SIZE;
	net_init_param.flags = 0;

	// try to init. If it's already inited we'll get SCE_NET_ERROR_EBUSY
	ret = sceNetInit(&net_init_param);
	if (ret < 0)
	{
		free(_vita_net_memory);
		_vita_net_memory = NULL;
		if (ret != SCE_NET_ERROR_EBUSY)
			return ret;
	}

	ret = sceNetCtlInit();
	if (ret < 0)
	{
		if (ret != SCE_NET_CTL_ERROR_NOT_TERMINATED)
		{
			// something gone wrong, tear down completely, so it can be re-inited
			// and we won't leak mem
			if (_vita_net_memory != NULL) // if we inited net successfully - cleanup
			{
				sceNetTerm();
				free(_vita_net_memory);
			}
			return ret;
		}
	}

	return 0;
}

