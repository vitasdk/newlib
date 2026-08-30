#ifndef _SYS_VITA_THREAD_H
#define _SYS_VITA_THREAD_H

/* 1: newlib owns a kernel TLS slot holding the per-thread pointers the thread
   library needs. 0: it owns no per-thread state and the thread library holds
   its own slot. Set here and not on the command line: newlib and everything
   built against it must agree, and only the installed header guarantees it. */
#define __VITA_NEWLIB_OWNS_THREAD_SLOT__ 1

#ifdef __cplusplus
extern "C" {
#endif

/* Exit the calling thread, reclaiming its newlib TLS-resident buffers first.
   Nothing can reach those from outside once the thread is gone. */
int vita_exit_thread(int exit_status);
int vita_exit_delete_thread(int exit_status);

#ifdef __cplusplus
}
#endif

#endif /* _SYS_VITA_THREAD_H */
