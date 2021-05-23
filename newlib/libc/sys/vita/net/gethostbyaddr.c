#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>
#include <psp2/net/net.h>

#define SCE_ERRNO_MASK 0xFF

struct hostent *gethostbyaddr(const void *__addr, socklen_t __len, int __type) {
    static struct hostent ent;
    char name[NI_MAXHOST];
    static char sname[NI_MAXHOST] = "";
    static char *addrlist[2] = { NULL, NULL };

    if (__type != SCE_NET_AF_INET) {
        errno = SCE_NET_ERROR_RESOLVER_ENOSUPPORT;
        return NULL;
    }

    int rid = sceNetResolverCreate("resolver", NULL, 0);
    if (rid < 0) {
        errno = rid & SCE_ERRNO_MASK;
        return NULL;
    }

    int err = sceNetResolverStartAton(rid, __addr, name, sizeof(name), 0, 0, 0);
    sceNetResolverDestroy(rid);
    if (err < 0) {
        errno = err & SCE_ERRNO_MASK;
        return NULL;
    }

    strncpy(sname, name, NI_MAXHOST - 1);
    addrlist[0] = (char *) __addr;

    ent.h_name = sname;
    ent.h_aliases = 0;
    ent.h_addrtype = __type;
    ent.h_length = sizeof(struct SceNetInAddr);
    ent.h_addr_list = addrlist;
    ent.h_addr = addrlist[0];

    return &ent;
}
