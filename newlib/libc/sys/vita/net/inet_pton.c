/*	$KAME: inet_pton.c,v 1.5 2001/08/20 02:32:40 itojun Exp $	*/

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
#include <string.h>
#include <sys/types.h>
#include <psp2/net/net.h>
#include "../vitanet.h"
#include "../vitaerror.h"

static int inet_pton4(const char *src, void *dst)
{
	int ret = sceNetInetPton(AF_INET, src, dst);
	if (!dst)
	{
		errno = __vita_scenet_errno_to_errno(scenet_errno);
	}
	return (ret < 0) ? -1 : ret;
}

static int inet_pton6(const char *src, u_char *dst)
{
	static const char xdigits_l[] = "0123456789abcdef";
	static const char xdigits_u[] = "0123456789ABCDEF";
	u_char tmp[NS_IN6ADDRSZ], *tp, *endp, *colonp;
	const char *xdigits, *curtok;
	int ch, saw_xdigit;
	u_int val;

	memset((tp = tmp), '\0', NS_IN6ADDRSZ);
	endp = tp + NS_IN6ADDRSZ;
	colonp = NULL;

	/* Leading :: requires some special handling. */
	if (*src == ':')
		if (*++src != ':')
			return 0;

	curtok = src;
	saw_xdigit = 0;
	val = 0;

	while ((ch = *src++) != '\0') {
		const char *pch;

		if ((pch = strchr((xdigits = xdigits_l), ch)) == NULL)
			pch = strchr((xdigits = xdigits_u), ch);

		if (pch != NULL) {
			val <<= 4;
			val |= (pch - xdigits);

			if (val > 0xffff)
				return 0;

			saw_xdigit = 1;
			continue;
		}

		if (ch == ':') {
			curtok = src;

			if (!saw_xdigit) {
				if (colonp)
					return 0;

				colonp = tp;
				continue;
			}

			if (tp + NS_INT16SZ > endp)
				return 0;

			*tp++ = (u_char) (val >> 8) & 0xff;
			*tp++ = (u_char) val & 0xff;
			saw_xdigit = 0;
			val = 0;
			continue;
		}

		if (ch == '.' && ((tp + NS_INADDRSZ) <= endp) &&
		        inet_pton4(curtok, tp) > 0) {
			tp += NS_INADDRSZ;
			saw_xdigit = 0;
			break;	/* '\0' was seen by inet_pton4(). */
		}

		return 0;
	}

	if (saw_xdigit) {
		if (tp + NS_INT16SZ > endp)
			return 0;

		*tp++ = (u_char) (val >> 8) & 0xff;
		*tp++ = (u_char) val & 0xff;
	}

	if (colonp != NULL) {
		/*
		 * Since some memmove()'s erroneously fail to handle
		 * overlapping regions, we'll do the shift by hand.
		 */
		const int n = tp - colonp;
		int i;

		for (i = 1; i <= n; i++) {
			endp[- i] = colonp[n - i];
			colonp[n - i] = 0;
		}

		tp = endp;
	}

	if (tp != endp)
		return 0;

	memcpy(dst, tmp, NS_IN6ADDRSZ);
	return 1;
}

int inet_pton(int af, const char *src, void *dst)
{
	switch (af) {
	case AF_INET:
		_vita_net_init();
		return inet_pton4(src, dst);

	case AF_INET6:
		_vita_net_init();
		return inet_pton6(src, dst);

	default:
		errno = EAFNOSUPPORT;
		return -1;
	}

	/* NOTREACHED */
}
