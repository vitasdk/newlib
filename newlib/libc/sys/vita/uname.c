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
#include <sys/utsname.h>
#include <string.h>
#include <stdio.h>

#include <psp2/kernel/modulemgr.h>

int uname(struct utsname *buf)
{
	if(buf == NULL)
	{
		errno = EFAULT;
		return -1;
	}
	strncpy(buf->sysname, "vita", __UTSNAMELEN-1);
	strncpy(buf->nodename, "localhost", __UTSNAMELEN-1);

	SceKernelSystemSwVersion version = {0};
	version.size = sizeof(version);
	sceKernelGetSystemSwVersion(&version);

	strncpy(buf->release, version.versionString, __UTSNAMELEN-1);
	snprintf(buf->version, __UTSNAMELEN, "PSP2 Kernel/VSH Version %s", version.versionString);
	strncpy(buf->machine, "armv7l", __UTSNAMELEN-1);

	return 0;
}
