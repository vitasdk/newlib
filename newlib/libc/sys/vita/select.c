/*

Copyright (C) 2016, Sunguk Lee

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
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include <psp2/kernel/threadmgr.h>
#include <psp2/net/net.h>
#include <psp2/types.h>

#include "vitadescriptor.h"

#define SCE_ERRNO_MASK 0xFF

#define MAX_EVENTS 255

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
		   struct timeval *timeout)
{
	uint64_t wait = timeout->tv_sec * 1000000 + timeout->tv_usec;

	if (nfds == 0) {
		sceKernelDelayThread(wait);
		return 0;
	}

	fd_set lock = {0};

	int i;
	int res;
	int eid = sceNetEpollCreate("", 0);
	if (eid < 0) {
		res = (int)eid;
		goto exit;
	}
	for (i = 0; i < nfds; i++) {
		SceNetEpollEvent ev = {0};
		ev.data.fd = i;
		if (readfds && FD_ISSET(i, readfds)) {
			ev.events |= SCE_NET_EPOLLIN;
		}
		if (writefds && FD_ISSET(i, writefds)) {
			ev.events |= SCE_NET_EPOLLOUT;
		}
		if (exceptfds && FD_ISSET(i, exceptfds)) {
			ev.events |= SCE_NET_EPOLLERR;
		}
		if (ev.events == 0) {
			continue;
		}

		DescriptorTranslation *fdmap = __vita_fd_grab(i);
		if (fdmap == NULL) {
			continue;
		}
		if (fdmap->type != VITA_DESCRIPTOR_SOCKET) {
			__vita_fd_drop(fdmap);
			continue;
		}
		FD_SET(i, &lock);

		sceNetEpollControl(eid, SCE_NET_EPOLL_CTL_ADD, fdmap->sce_uid, &ev);
	}
	SceNetEpollEvent events[MAX_EVENTS] = {0};
	int nev = sceNetEpollWait(eid, events, MAX_EVENTS, wait);

	if (nev < 0) {
		res = nev;
		goto clean;
	}

	if (readfds) {
		FD_ZERO(readfds);
	}
	if (writefds) {
		FD_ZERO(writefds);
	}
	if (exceptfds) {
		FD_ZERO(exceptfds);
	}

	res = 0;

	for (i = 0; i < nev; i++) {
		if (events[i].events) {
			if (events[i].events & SCE_NET_EPOLLIN) {
				FD_SET(events[i].data.fd, readfds);
				res++;
			}
			if (events[i].events & SCE_NET_EPOLLOUT) {
				FD_SET(events[i].data.fd, writefds);
				res++;
			}
			if (events[i].events & SCE_NET_EPOLLERR) {
				FD_SET(events[i].data.fd, exceptfds);
				res++;
			}
		}
	}

clean:
	sceNetEpollDestroy(eid);
exit:
	for (i = 0; i < nfds; i++) {
		if (FD_ISSET(i, &lock) && __vita_fdmap[i] != NULL) {
			__vita_fd_drop(__vita_fdmap[i]);
		}
	}

	if (res < 0) {
		errno = res & SCE_ERRNO_MASK;
		return -1;
	}
	return res;
}
