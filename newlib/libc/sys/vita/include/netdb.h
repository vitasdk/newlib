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
#ifndef _NETDB_H_
#define _NETDB_H_

#include <sys/socket.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct hostent {
	char *h_name;
	char **h_aliases;
	int h_addrtype;
	int h_length;
	char **h_addr_list;
};
#define h_addr h_addr_list[0]

struct servent {
	char *s_name;
	char **s_aliases;
	int s_port;
	char *s_proto;
};

struct addrinfo {
	int ai_flags;
	int ai_family;
	int ai_socktype;
	int ai_protocol;
	size_t ai_addrlen;
	char *ai_canonname;
	struct sockaddr *ai_addr;
	struct addrinfo *ai_next;
};

/* Error return codes from getaddrinfo() */
#define EAI_BADFLAGS        -1      /* Invalid value for `ai_flags' field.  */
#define EAI_NONAME          -2      /* NAME or SERVICE is unknown.  */
#define EAI_AGAIN           -3      /* Temporary failure in name resolution.  */
#define EAI_FAIL            -4      /* Non-recoverable failure in name res.  */
#define EAI_NODATA          -5      /* No address associated with NAME.  */
#define EAI_FAMILY          -6      /* `ai_family' not supported.  */
#define EAI_SOCKTYPE        -7      /* `ai_socktype' not supported.  */
#define EAI_SERVICE         -8      /* SERVICE not supported for `ai_socktype'.  */
#define EAI_ADDRFAMILY      -9      /* Address family for NAME not supported.  */
#define EAI_MEMORY          -10     /* Memory allocation failure.  */
#define EAI_SYSTEM          -11     /* System error returned in `errno'.  */
#define EAI_OVERFLOW        -12     /* Argument buffer overflow.  */
#ifdef __USE_GNU
#define EAI_INPROGRESS      -100    /* Processing request in progress.  */
#define EAI_CANCELED        -101    /* Request canceled.  */
#define EAI_NOTCANCELED     -102    /* Request not canceled.  */
#define EAI_ALLDONE         -103    /* All requests done.  */
#define EAI_INTR            -104    /* Interrupted by a signal.  */
#define EAI_IDN_ENCODE      -105    /* IDN encoding failed.  */
#endif

/* Flag values for getaddrinfo() */
#define AI_PASSIVE          0x00000001      /* Get address to use bind() */
#define AI_CANONNAME        0x00000002      /* Fill ai_canonname */
#define AI_NUMERICHOST      0x00000004      /* Prevent name resolution */
#define AI_NUMERICSERV      0x00000008      /* Fon't use name resolution. */

/* Valid flags for addrinfo */
#define AI_MASK             (AI_PASSIVE | AI_CANONNAME | AI_NUMERICHOST | AI_ADDRCONFIG)
#define AI_ALL              0x00000100      /* IPv6 and IPv4-mapped (with AI_V4MAPPED) */
#define AI_V4MAPPED_CFG     0x00000200      /* Accept IPv4-mapped if kernel supports */
#define AI_ADDRCONFIG       0x00000400      /* Only if any address is assigned */
#define AI_V4MAPPED         0x00000800      /* Accept IPv4-mapped IPv6 address */

struct hostent *gethostbyname(const char *name);
struct servent *getservbyname(const char *name, const char *proto);
int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);
void freeaddrinfo(struct addrinfo *res);

#ifdef __cplusplus
}
#endif

#endif
