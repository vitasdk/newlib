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

#define ntohs __builtin_bswap16
#define htons __builtin_bswap16
#define ntohl __builtin_bswap32
#define htonl __builtin_bswap32

#endif
