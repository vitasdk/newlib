// Newlib's own reentrancy (struct _reent) lives directly in native ELF TLS
// now (see --enable-newlib-reent-thread-local), so this file only keeps the
// per-thread pointer slots pthread-embedded still needs on top of that.
// Native TLS can't replace those: pte_osThreadCreate writes into a new
// thread's slot through sceKernelGetThreadTLSAddr(thid, ...) before that
// thread has even started, and there is no way to reach another thread's
// TPIDRURO-relative TLS block from outside it, especially pre-start.

#include <reent.h>
#include <string.h>

#include <vitasdk/utils.h>
#include <psp2/kernel/threadmgr.h>

// not in sdk
void sceClibPrintf(const char *fmt, ...);

#define MAX_THREADS 256

typedef struct thread_ext_data {
	int thread_id;
	void *tls_data_ext;
	void *pthread_data_ext;
} thread_ext_data;

static thread_ext_data thread_ext_list[MAX_THREADS];
static int _newlib_reent_mutex;

#define TLS_EXT_THID_PTR(thid)	sceKernelGetThreadTLSAddr(thid, 0x89)
#define TLS_EXT_PTR				sceKernelGetTLSAddr(0x89)

int __vita_delete_thread_reent(int thid)
{
	struct thread_ext_data **on_tls = NULL;
	struct thread_ext_data *for_thread;

	if (thid == 0)
		on_tls = TLS_EXT_PTR;
	else
		on_tls = TLS_EXT_THID_PTR(thid);

	if (!*on_tls)
		return 0;

	for_thread = *on_tls;

	// Remove from TLS
	*on_tls = 0;

	// Set thread id to zero, which means the slot is free
	for_thread->thread_id = 0;

	return 1;
}

int vitasdk_delete_thread_reent(int thid)
{
	int res = 0;
	// Lock the list because we'll be modifying it
	sceKernelLockMutex(_newlib_reent_mutex, 1, NULL);

	res = __vita_delete_thread_reent(thid);

	sceKernelUnlockMutex(_newlib_reent_mutex, 1);
	return res;
}

int _exit_thread_common(int exit_status, int (*exit_func)(int))
{
	// Reclaim this thread's own newlib TLS-resident buffers (mprec bigints,
	// _cvtbuf, locale, ...) before its TLS block disappears with it: unlike
	// the old reent pool, nothing can reach these from outside the thread
	// once it's gone.
	_reclaim_reent(NULL);

	// Do NOT hold _newlib_reent_mutex across this call: exit_func normally
	// never returns, which would leave the mutex permanently owned by a
	// dead thread and deadlock the next vitasdk_get_tls_data/
	// vitasdk_get_pthread_data/vitasdk_delete_thread_reent call on any
	// other thread. The thread's slot doesn't need clearing here either -
	// pte_osThreadDelete already does that via vitasdk_delete_thread_reent
	// before a normal exit-and-delete, and __vita_clean_thread_ext's
	// liveness scan reclaims anything left over from threads that exited
	// without going through it.
	return exit_func(exit_status);
}

int vita_exit_thread(int exit_status)
{
	return _exit_thread_common(exit_status, sceKernelExitThread);
}

int vita_exit_delete_thread(int exit_status)
{
	return _exit_thread_common(exit_status, sceKernelExitDeleteThread);
}

static inline void __vita_clean_thread_ext(void)
{
	int i;
	SceKernelThreadInfo info;

	for (i = 0; i < MAX_THREADS; ++i)
	{
		info.size = sizeof(SceKernelThreadInfo);

		if (sceKernelGetThreadInfo(thread_ext_list[i].thread_id, &info) < 0)
		{
			thread_ext_list[i].thread_id = 0;
		}
	}
}

static inline struct thread_ext_data *__vita_allocate_thread_ext(void)
{
	int i;

	for (i = 0; i < MAX_THREADS; ++i)
		if (thread_ext_list[i].thread_id == 0)
			return &thread_ext_list[i];

	return 0;
}

static struct thread_ext_data *__vita_thread_ext(int thid)
{
	struct thread_ext_data **on_tls = NULL;
	struct thread_ext_data *slot;

	if (thid == 0)
		on_tls = TLS_EXT_PTR;
	else
		on_tls = TLS_EXT_THID_PTR(thid);

	if (*on_tls)
	{
		return *on_tls;
	}

	sceKernelLockMutex(_newlib_reent_mutex, 1, 0);

	// If it's not on the TLS this means the thread doesn't have a slot
	// allocated yet. We allocate one and put a pointer to it on the TLS.
	slot = __vita_allocate_thread_ext();

	if (!slot)
	{
		// clean any hanging thread references
		__vita_clean_thread_ext();

		slot = __vita_allocate_thread_ext();

		if (!slot)
		{
			// we've exhausted all our resources
			sceClibPrintf("[VITASDK] FATAL: Exhausted all thread data resources!");
			__builtin_trap();
		}
	}

	memset(slot, 0, sizeof(*slot));
	slot->thread_id = (thid == 0) ? sceKernelGetThreadId() : thid;

	// Put it on TLS for faster access time
	*on_tls = slot;

	sceKernelUnlockMutex(_newlib_reent_mutex, 1);
	return slot;
}

void *vitasdk_get_tls_data(SceUID thid)
{
	return &__vita_thread_ext(thid)->tls_data_ext;
}

void *vitasdk_get_pthread_data(SceUID thid)
{
	return &__vita_thread_ext(thid)->pthread_data_ext;
}

// Called from _start to set up the main thread's slot
void _init_vita_reent(void)
{
	memset(thread_ext_list, 0, sizeof(thread_ext_list));
	_newlib_reent_mutex = sceKernelCreateMutex("thread ext data access mutex", 0, 0, 0);
	thread_ext_list[0].thread_id = sceKernelGetThreadId();
	*(struct thread_ext_data **)(TLS_EXT_PTR) = &thread_ext_list[0];
}

void _free_vita_reent(void)
{
	sceKernelDeleteMutex(_newlib_reent_mutex);
}
