/* Copyright (c) 1996 by Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL INTERNET SOFTWARE
 * CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

/* Author: Paul Vixie, 1996 */
/* Copied from Linux, modified for Phoenix-RTOS. */

#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <psp2/net/net.h>
#include "../vitanet.h"
#include "../vitaerror.h"

#define SPRINTF(x) ((socklen_t) sprintf x)

static const char *inet_ntop4(const void *src, char *dst, socklen_t size)
{
	const char* ret = sceNetInetNtop(AF_INET, src, dst, size);
	if (!ret)
	{
		errno = __vita_scenet_errno_to_errno(scenet_errno);
	}
	return ret;
}

static const char *inet_ntop6(const u_char *src, char *dst, socklen_t size)
{
	/*
	 * Note that int32_t and int16_t need only be "at least" large enough
	 * to contain a value of the specified size.  On some systems, like
	 * Crays, there is no such thing as an integer variable with 16 bits.
	 * Keep this in mind if you think this function should have been coded
	 * to use pointer overlays.  All the world's not a VAX.
	 */
	char tmp[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")], *tp;
	struct {
		int base, len;
	} best, cur;
	u_int words[NS_IN6ADDRSZ / NS_INT16SZ];
	int i;
	/*
	 * Preprocess:
	 *	Copy the input (bytewise) array into a wordwise array.
	 *	Find the longest run of 0x00's in src[] for :: shorthanding.
	 */
	memset(words, '\0', sizeof words);

	for (i = 0; i < NS_IN6ADDRSZ; i++)
		words[i / 2] |= (src[i] << ((1 - (i % 2)) << 3));

	best.base = -1;
	cur.base = -1;

	for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
		if (words[i] == 0) {
			if (cur.base == -1)
				cur.base = i, cur.len = 1;

			else
				cur.len++;
		}

		else {
			if (cur.base != -1) {
				if (best.base == -1 || cur.len > best.len)
					best = cur;

				cur.base = -1;
			}
		}
	}

	if (cur.base != -1) {
		if (best.base == -1 || cur.len > best.len)
			best = cur;
	}

	if (best.base != -1 && best.len < 2)
		best.base = -1;

	/*
	 * Format the result.
	 */
	tp = tmp;

	for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
		/* Are we inside the best run of 0x00's? */
		if (best.base != -1 && i >= best.base &&
		        i < (best.base + best.len)) {
			if (i == best.base)
				*tp++ = ':';

			continue;
		}

		/* Are we following an initial run of 0x00s or any real hex? */
		if (i != 0)
			*tp++ = ':';

		/* Is this address an encapsulated IPv4? */
		if (i == 6 && best.base == 0 &&
		        (best.len == 6 || (best.len == 5 && words[5] == 0xffff))) {
			if (!inet_ntop4(src + 12, tp, sizeof tmp - (tp - tmp)))
				return NULL;

			tp += strlen(tp);
			break;
		}

		tp += SPRINTF((tp, "%x", words[i]));
	}

	/* Was it a trailing run of 0x00's? */
	if (best.base != -1 && (best.base + best.len) ==
	        (NS_IN6ADDRSZ / NS_INT16SZ))
		*tp++ = ':';

	*tp++ = '\0';

	/*
	 * Check for overflow, copy, and we're done.
	 */
	if ((socklen_t)(tp - tmp) > size) {
		errno = ENOSPC;
		return NULL;
	}

	strcpy(dst, tmp);
	return dst;
}

const char *inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
	switch (af) {
	case AF_INET:
		_vita_net_init();
		return inet_ntop4(src, dst, size);

	case AF_INET6:
		_vita_net_init();
		return inet_ntop6(src, dst, size);

	default:
		errno = EAFNOSUPPORT;
		return NULL;
	}

	/* NOTREACHED */
}
