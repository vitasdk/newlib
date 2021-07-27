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
#include <reent.h>
   
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
   
int initgroups(const char *user, gid_t group)
{
	struct _reent *reent = _REENT;
	reent->_errno = ENOSYS;
	return -1;
}

int getgroups(int size, gid_t list[])
{
	struct _reent *reent = _REENT;
	reent->_errno = ENOSYS;
	return -1;
}

int setgroups(size_t size, const gid_t *list)
{
	struct _reent *reent = _REENT;
	reent->_errno = ENOSYS;
	return -1;
}

struct group *getgrent()
{
	/* TODO: implement. */
	struct _reent *reent = _REENT;
	reent->_errno = ENOSYS;
	return NULL;
}

void setgrent()
{
	/* TODO: implement. */
}

void endgrent()
{
	/* TODO: implement. */
}

struct group *getgrnam(const char *name)
{
	/* TODO: implement. */
	struct _reent *reent = _REENT;
	reent->_errno = ENOSYS;
	return NULL;
}

struct group *getgrgid(gid_t gid)
{
	/* TODO: implement. */
	struct _reent *reent = _REENT;
	reent->_errno = ENOSYS;
	return NULL;
}

int setpgrp(pid_t pid, pid_t pgid)
{
	/* TODO: implement. */
	struct _reent *reent = _REENT;
	reent->_errno = ENOSYS;
	return -1;
}
