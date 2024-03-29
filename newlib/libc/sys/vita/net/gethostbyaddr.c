#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>
#include <psp2/net/net.h>
#include "../vitaerror.h"
#include "../vitanet.h"

struct hostent *gethostbyaddr(const void *addr, socklen_t len, int type)
{
	_vita_net_init();
	static struct hostent ent;
	char name[NI_MAXHOST];
	static char sname[NI_MAXHOST] = "";
	static char *addrlist[2] = { NULL, NULL };

	if (type != SCE_NET_AF_INET)
	{
		errno = SCE_NET_ERROR_RESOLVER_ENOSUPPORT;
		return NULL;
	}

	int rid = sceNetResolverCreate("resolver", NULL, 0);
	if (rid < 0)
	{
		errno = __vita_scenet_errno_to_errno(rid);
		return NULL;
	}

	int err = sceNetResolverStartAton(rid, addr, name, sizeof(name), 0, 0, 0);
	sceNetResolverDestroy(rid);
	if (err < 0)
	{
		errno = __vita_scenet_errno_to_errno(err);
		return NULL;
	}

	strncpy(sname, name, NI_MAXHOST - 1);
	addrlist[0] = (char *) addr;

	ent.h_name = sname;
	ent.h_aliases = 0;
	ent.h_addrtype = type;
	ent.h_length = sizeof(struct SceNetInAddr);
	ent.h_addr_list = addrlist;
	ent.h_addr = addrlist[0];

	return &ent;
}
