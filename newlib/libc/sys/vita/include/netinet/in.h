/*
 * Copyright (c) 2016, 2017, 2018 vitasdk
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
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _NETINET_IN_H_
#define _NETINET_IN_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <psp2/net/net.h>

#define IPPROTO_IP		SCE_NET_IPPROTO_IP
#define IPPROTO_ICMP		SCE_NET_IPPROTO_ICMP
#define IPPROTO_IGMP		SCE_NET_IPPROTO_IGMP
#define IPPROTO_TCP		SCE_NET_IPPROTO_TCP
#define IPPROTO_UDP		SCE_NET_IPPROTO_UDP

typedef uint32_t in_addr_t;
typedef uint16_t in_port_t;

typedef struct in_addr {
	in_addr_t s_addr;
} in_addr;

typedef struct sockaddr_in {
	uint8_t			sin_len;
	sa_family_t		sin_family;
	in_port_t		sin_port;
	struct in_addr		sin_addr;
	in_port_t		sin_vport;
	char			sin_zero[6];
} sockaddr_in;

/* Address to accept any incoming messages. */
#define	INADDR_ANY		((in_addr_t) 0x00000000)

/* Address to send to all hosts. */
#define	INADDR_BROADCAST	((in_addr_t) 0xffffffff)

#define	INADDR_LOOPBACK		((in_addr_t) 0x7f000001) // 127.0.0.1
#define	INADDR_NONE		((in_addr_t) 0xffffffff)

#define	IN_CLASSA(i)		(((u_int32_t)(i) & 0x80000000) == 0)
#define	IN_CLASSA_NET		0xff000000
#define	IN_CLASSA_NSHIFT	24
#define	IN_CLASSA_HOST		0x00ffffff
#define	IN_CLASSA_MAX		128

#define	IN_CLASSB(i)		(((u_int32_t)(i) & 0xc0000000) == 0x80000000)
#define	IN_CLASSB_NET		0xffff0000
#define	IN_CLASSB_NSHIFT	16
#define	IN_CLASSB_HOST		0x0000ffff
#define	IN_CLASSB_MAX		65536

#define	IN_CLASSC(i)		(((u_int32_t)(i) & 0xe0000000) == 0xc0000000)
#define	IN_CLASSC_NET		0xffffff00
#define	IN_CLASSC_NSHIFT	8
#define	IN_CLASSC_HOST		0x000000ff

#define	IN_CLASSD(i)		(((u_int32_t)(i) & 0xf0000000) == 0xe0000000)
#define	IN_CLASSD_NET		0xf0000000	/* These ones aren't really */
#define	IN_CLASSD_NSHIFT	28		/* net and host fields, but */
#define	IN_CLASSD_HOST		0x0fffffff	/* routing needn't know.    */
#define	IN_MULTICAST(i)		IN_CLASSD(i)

#define	IN_EXPERIMENTAL(i)	(((u_int32_t)(i) & 0xf0000000) == 0xf0000000)
#define	IN_BADCLASS(i)		(((u_int32_t)(i) & 0xf0000000) == 0xf0000000)


#define	INADDR_UNSPEC_GROUP	(u_int32_t)0xe0000000	/* 224.0.0.0 */
#define	INADDR_ALLHOSTS_GROUP	(u_int32_t)0xe0000001	/* 224.0.0.1 */
#define	INADDR_ALLRTRS_GROUP	(u_int32_t)0xe0000002	/* 224.0.0.2 */
#define	INADDR_MAX_LOCAL_GROUP	(u_int32_t)0xe00000ff	/* 224.0.0.255 */

#define	IN_LOOPBACKNET		127			/* official! */

#define ntohs __builtin_bswap16
#define htons __builtin_bswap16
#define ntohl __builtin_bswap32
#define htonl __builtin_bswap32

#endif
