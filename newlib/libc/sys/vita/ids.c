/*

Copyright (C) 2021, vitasdk

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
#include <sys/types.h>
#include <unistd.h>

uid_t getuid()
{
	return 1;
}

uid_t geteuid()
{
	return 1;
}

int setuid(uid_t uid)
{
	errno = ENOSYS;
	return -1;
}

int seteuid(uid_t euid)
{
	errno = ENOSYS;
	return -1;
}

int setreuid(uid_t ruid, uid_t euid)
{
	errno = ENOSYS;
	return -1;
}

gid_t getgid()
{
	return 1;
}

gid_t getegid()
{
	return 1;
}

int setgid(gid_t gid)
{
	errno = ENOSYS;
	return -1;
}

int setegid(gid_t egid)
{
	errno = ENOSYS;
	return -1;
}

int setregid(gid_t rgid, gid_t egid)
{
	errno = ENOSYS;
	return -1;
}

int setpgid(pid_t pid, pid_t pgid)
{
	errno = ENOSYS;
	return -1;
}

pid_t getsid(pid_t pid)
{
	errno = ENOSYS;
	return -1;
}

pid_t setsid()
{
	errno = ENOSYS;
	return -1;
}
