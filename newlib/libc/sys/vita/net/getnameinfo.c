/*

Copyright (C) 2026, vitasdk

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

#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <psp2/net/net.h>
#include "../vitaerror.h"
#include "../vitanet.h"

int _get_host_info(const struct sockaddr* addr, socklen_t addrlen,  char* host, socklen_t hostlen, int flags)
{
    _vita_net_init();

    const struct sockaddr_in *sinp = (const struct sockaddr_in *)addr;

    if (!(flags & NI_NUMERICHOST))
    {
        int rid = sceNetResolverCreate("resolver", NULL, 0);
        if (rid < 0)
        {
            return EAI_SYSTEM;
        }

        char name[NI_MAXHOST];
        int err = sceNetResolverStartAton(rid, (const SceNetInAddr *)&sinp->sin_addr, name, sizeof(name), 0, 0, 0);
        sceNetResolverDestroy(rid);
        if (err < 0)
        {
            if (flags & NI_NAMEREQD)
               return EAI_NONAME;
        }
        if (strlen(name) > hostlen -1)
            return EAI_OVERFLOW;
        strncpy(host, name, hostlen - 1);
        host[hostlen - 1] = '\0';
    }

    if (inet_ntop(AF_INET, &sinp->sin_addr, host, hostlen) == NULL)
        return EAI_OVERFLOW;

    return 0;
}

int _get_serv_info(const struct sockaddr* addr, socklen_t addrlen, char* serv, socklen_t servlen, int flags)
{
    const struct sockaddr_in *sinp = (const struct sockaddr_in *)addr;
    int ret = snprintf(serv, servlen, "%d", ntohs (sinp->sin_port));
    if (ret < 0) return EAI_SYSTEM;
    if (ret >= servlen) return EAI_OVERFLOW;
    return 0;
}

int getnameinfo(const struct sockaddr* addr, socklen_t addrlen,  char* host, socklen_t hostlen, char* serv, socklen_t servlen, int flags)
{

    if (flags & ~(NI_NUMERICHOST|NI_NUMERICSERV|NI_NOFQDN|NI_NAMEREQD|NI_DGRAM))
        return EAI_BADFLAGS;

    if (addr == NULL || addrlen < sizeof (sa_family_t))
        return EAI_FAMILY;

    if ((flags & NI_NAMEREQD) && host == NULL && serv == NULL)
        return EAI_NONAME;

    switch (addr->sa_family)
    {
        case AF_INET:
            if (addrlen < sizeof(struct sockaddr_in))
                return EAI_FAMILY;
            break;
        case AF_LOCAL:
        case AF_INET6:
        default:
            return EAI_FAMILY;
            break;
    }


    if (host != NULL && hostlen > 0)
    {
        int result = _get_host_info(addr, addrlen, host, hostlen, flags);
        if (result != 0)
        {
            return result;
        }
    }

    if (serv && (servlen > 0))
    {
        int result = _get_serv_info(addr, addrlen, serv, servlen, flags);
        if (result != 0)
        {
            return result;
        }
    }

    return 0;
}
