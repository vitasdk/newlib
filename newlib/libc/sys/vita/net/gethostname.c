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

#include "../vitaerror.h"
#include "../vitanet.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <psp2/net/netctl.h>

int gethostname(char *name, size_t len)
{
	_vita_net_init();
	if (!name)
	{
		errno = EFAULT;
		return -1;
	}
	if (len < 0)
	{
		errno = EINVAL;
		return -1;
	}

	memset(name, 0, len);

	SceNetCtlInfo info;
	int ret = sceNetCtlInetGetInfo(SCE_NETCTL_INFO_GET_DHCP_HOSTNAME, &info);

	if (ret < 0)
	{
		errno = __vita_scenet_errno_to_errno(ret);
		return -1;
	}

	if (strlen(info.dhcp_hostname) > 0)
	{
		strncpy(name, info.dhcp_hostname, len-1);
		return 0;
	}

	// fallback to "localhost"
	strncpy(name, "localhost", len-1);
	return 0;
}

int sethostname(const char *name, size_t len)
{
	errno = EPERM;
	return -1;
}