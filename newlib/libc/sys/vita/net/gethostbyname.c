#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <psp2/net/net.h>
#include "../vitaerror.h"
#include "../vitanet.h"

#define MAX_NAME 512

struct hostent *gethostbyname(const char *name)
{
	_vita_net_init();
	static struct hostent ent;
	static char sname[MAX_NAME] = "";
	static struct SceNetInAddr saddr = {0};
	static char *addrlist[2] = { (char *) &saddr, NULL };

	int rid = sceNetResolverCreate("resolver", NULL, 0);
	if (rid < 0)
	{
		errno = __vita_scenet_errno_to_errno(rid);
		return NULL;
	}

	int err = sceNetResolverStartNtoa(rid, name, &saddr, 0, 0, 0);
	sceNetResolverDestroy(rid);
	if (err < 0)
	{
		errno = __vita_scenet_errno_to_errno(err);
		return NULL;
	}

	strncpy(sname, name, MAX_NAME - 1);

	ent.h_name = sname;
	ent.h_aliases = 0;
	ent.h_addrtype = SCE_NET_AF_INET;
	ent.h_length = sizeof(struct SceNetInAddr);
	ent.h_addr_list = addrlist;
	ent.h_addr = addrlist[0];

	return &ent;
}
