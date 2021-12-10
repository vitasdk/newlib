/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 1997 Peter Wemm <peter@freebsd.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD$
 *
 * Modified for vita
 */

#ifndef _SYS_POLL_H_
#define	_SYS_POLL_H_

#include <sys/cdefs.h>

/*
 * This file is intended to be compatible with the traditional poll.h.
 */

typedef	unsigned int	nfds_t;

/*
 * This structure is passed as an array to poll(2).
 */
struct pollfd {
	int	fd;		/* which file descriptor to poll */
	short	events;		/* events we are interested in */
	short	revents;	/* events found on return */
};

/*
 * Requestable events.  If poll(2) finds any of these set, they are
 * copied to revents on return.
 */

/*
 * There are no priorities on vita, only IN and OUT
 */
#define	POLLIN		0x0001		/* any readable data available */
#define	POLLPRI		POLLIN		/* OOB/Urgent readable data */
#define	POLLOUT		0x0004		/* file descriptor is writeable */

#define	POLLRDNORM	POLLIN		/* non-OOB/URG data available */
#define	POLLRDBAND	POLLIN		/* OOB/Urgent readable data */
#define	POLLWRNORM	POLLOUT		/* non-OOB/URG data can be written */
#define	POLLWRBAND	POLLOUT		/* OOB/Urgent data can be written */

/*
 * These events are set if they occur regardless of whether they were
 * requested.
 */
#define	POLLERR		0x0008		/* some poll error occurred */
#define	POLLHUP		0x0010		/* file descriptor was "hung up" */
#define	POLLNVAL	0x0020		/* requested events "invalid" */

__BEGIN_DECLS
int	poll(struct pollfd _pfd[], nfds_t _nfds, int _timeout);
__END_DECLS

#endif /* !_SYS_POLL_H_ */
