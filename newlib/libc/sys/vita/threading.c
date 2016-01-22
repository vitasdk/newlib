// This provides support for __getreent() as well as implementation of our thread-related wrappers

#include <reent.h>

#define MAX_THREADS 64

typedef struct reent_for_thread {
	int thread_id;
	int needs_reclaim;
	struct _reent reent;
} reent_for_thread;

static reent_for_thread reent_list[MAX_THREADS];
static int _newlib_reent_mutex;
static struct _reent _newlib_global_reent;

int sceKernelExitThread(int);
int sceKernelExitDeleteThread(int);

#define TLS_REENT_PTR sceKernelGetTLSAddr(0x88)

#define list_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

int _exit_thread_common(int exit_status, int (*exit_func)(int)) {
	int ret, thread_id;
	struct reent_for_thread *for_thread;
	// We only need to cleanup if reent is allocated, i.e. if it's on our TLS
	// We also don't need to clean up the global reent
	struct _reent **on_tls = TLS_REENT_PTR;
	if (!*on_tls || *on_tls == &_newlib_global_reent)
		return exit_func(exit_status);
	for_thread = list_entry(*on_tls, struct reent_for_thread, reent);
	// Remove from TLS
	*on_tls = 0;
	// Lock the list because we'll be modifying it
	sceKernelLockMutex(_newlib_reent_mutex);
	// Set thread id to zero, which means the reent is free
	thread_id = for_thread->thread_id;
	for_thread->thread_id = 0;
	// We can't reclaim it here, will be done later in __getreent
	for_thread->needs_reclaim = 1;
	// Try to actually exit the thread
	ret = exit_func(exit_status);
	// If it succeeds, the mutex is unlocked, and we don't get here
	// However, if it fails, we need to restore thread_id and unlock the mutex
	for_thread->thread_id = thread_id;
	sceKernelUnlockMutex(_newlib_reent_mutex);
	// And put it back on TLS
	*on_tls = &for_thread->reent;
	return ret;
}

int vita_exit_thread(int exit_status) {
	return _exit_thread_common(exit_status, sceKernelExitThread);
}

int vita_exit_delete_thread(int exit_status) {
	return _exit_thread_common(exit_status, sceKernelExitDeleteThread);
}

struct _reent *__getreent(void) {
	int i;
	struct reent_for_thread *free_reent = 0;
	struct _reent *returned_reent = 0;
	// A pointer to our reent should be on the TLS
	struct _reent **on_tls = TLS_REENT_PTR;
	if (*on_tls) {
		return *on_tls;
	}
	// If it's not on the TLS this means the thread doesn't have a reent allocated yet
	// We allocate one and put a pointer to it on the TLS
	sceKernelLockMutex(_newlib_reent_mutex, 1, 0);
	for (i = 0; i < MAX_THREADS; ++i)
		if (reent_list[i].thread_id == 0) {
			free_reent = &reent_list[i];
			break;
		}
	if (!free_reent) {
		returned_reent = &_newlib_global_reent;
	} else {
		// First, check if it needs to be cleaned up (if it came from another thread)
		if (free_reent->needs_reclaim) {
			_reclaim_reent(&free_reent->reent);
			free_reent->needs_reclaim = 0;
		}
		// Set it up
		free_reent->thread_id = sceKernelGetThreadId();
		_REENT_INIT_PTR(&free_reent->reent);
		returned_reent = &free_reent->reent;
	}
	sceKernelUnlockMutex(_newlib_reent_mutex, 1);
	// Put it on TLS for faster access time
	*on_tls = returned_reent;
	return returned_reent;
}

// Called from _start to set up the main thread reentrancy structure
void _init_vita_reent(void) {
	_newlib_reent_mutex = sceKernelCreateMutex("reent list access mutex", 0, 0, 0);
	reent_list[0].thread_id = sceKernelGetThreadId();
	_REENT_INIT_PTR(&reent_list[0].reent);
	*(struct _reent **)(TLS_REENT_PTR) = &reent_list[0].reent;
	_REENT_INIT_PTR(&_newlib_global_reent);
}

void _free_vita_reent(void) {
	sceKernelDeleteMutex(_newlib_reent_mutex);
}
