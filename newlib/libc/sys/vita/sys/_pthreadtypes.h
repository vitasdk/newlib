/*
 *  Written by Joel Sherrill <joel.sherrill@OARcorp.com>.
 *
 *  COPYRIGHT (c) 1989-2013, 2015.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Permission to use, copy, modify, and distribute this software for any
 *  purpose without fee is hereby granted, provided that this entire notice
 *  is included in all copies of any software which is or includes a copy
 *  or modification of this software.
 *
 *  THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
 *  WARRANTY.  IN PARTICULAR,  THE AUTHOR MAKES NO REPRESENTATION
 *  OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY OF THIS
 *  SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.
 */

#ifndef _SYS__PTHREADTYPES_H_
#define	_SYS__PTHREADTYPES_H_

#if defined(_POSIX_THREADS) || __POSIX_VISIBLE >= 199506

#include <sys/sched.h>

/*
 *  2.5 Primitive System Data Types,  P1003.1c/D10, p. 19.
 */

#ifdef _POSIX_THREADS_INTERNAL
typedef struct pthread_t_ * pthread_t;            /* identify a thread */
#else
typedef __uint32_t pthread_t;            /* identify a thread */
#endif

/* P1003.1c/D10, p. 118-119 */
#define PTHREAD_SCOPE_PROCESS 0
#define PTHREAD_SCOPE_SYSTEM  1

/* P1003.1c/D10, p. 111 */
#define PTHREAD_INHERIT_SCHED  1      /* scheduling policy and associated */
                                      /*   attributes are inherited from */
                                      /*   the calling thread. */
#define PTHREAD_EXPLICIT_SCHED 2      /* set from provided attribute object */

/* P1003.1c/D10, p. 141 */
#define PTHREAD_CREATE_DETACHED 0
#define PTHREAD_CREATE_JOINABLE  1

typedef struct pthread_attr_t_ * pthread_attr_t;

#if defined(_POSIX_THREAD_PROCESS_SHARED)
/* NOTE: P1003.1c/D10, p. 81 defines following values for process_shared.  */

#define PTHREAD_PROCESS_PRIVATE 0 /* visible within only the creating process */
#define PTHREAD_PROCESS_SHARED  1 /* visible too all processes with access to */
                                  /*   the memory where the resource is */
                                  /*   located */
#endif

#if defined(_POSIX_THREAD_PRIO_PROTECT)
/* Mutexes */

/* Values for blocking protocol. */

#define PTHREAD_PRIO_NONE    0
#define PTHREAD_PRIO_INHERIT 1
#define PTHREAD_PRIO_PROTECT 2
#endif

enum
{
    /* Compatibility with LinuxThreads */
    PTHREAD_MUTEX_FAST_NP,
    PTHREAD_MUTEX_RECURSIVE_NP,
    PTHREAD_MUTEX_ERRORCHECK_NP,
    PTHREAD_MUTEX_TIMED_NP = PTHREAD_MUTEX_FAST_NP,
    PTHREAD_MUTEX_ADAPTIVE_NP = PTHREAD_MUTEX_FAST_NP,
    /* For compatibility with POSIX */
    PTHREAD_MUTEX_NORMAL = PTHREAD_MUTEX_FAST_NP,
    PTHREAD_MUTEX_RECURSIVE = PTHREAD_MUTEX_RECURSIVE_NP,
    PTHREAD_MUTEX_ERRORCHECK = PTHREAD_MUTEX_ERRORCHECK_NP,
    PTHREAD_MUTEX_DEFAULT = PTHREAD_MUTEX_NORMAL
};

typedef struct pthread_mutex_t_ * pthread_mutex_t;
typedef struct pthread_mutexattr_t_ * pthread_mutexattr_t;
#define _PTHREAD_MUTEX_INITIALIZER ((pthread_mutex_t) -1)
#define PTHREAD_RECURSIVE_MUTEX_INITIALIZER ((pthread_mutex_t) -2)
#define PTHREAD_ERRORCHECK_MUTEX_INITIALIZER ((pthread_mutex_t) -3)

/*
* Compatibility with LinuxThreads
*/
#define PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP PTHREAD_RECURSIVE_MUTEX_INITIALIZER
#define PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP PTHREAD_ERRORCHECK_MUTEX_INITIALIZER

/* Condition Variables */

typedef struct pthread_cond_t_ * pthread_cond_t;
typedef struct pthread_condattr_t_ * pthread_condattr_t;
#define _PTHREAD_COND_INITIALIZER ((pthread_cond_t) -1)


/* Keys */
struct pthread_once_t_ {
    int          state;
    void *       semaphore;
    int          numSemaphoreUsers;
    int          done;        /* indicates if user function has been executed */
};
typedef struct pthread_once_t_ pthread_once_t;
#define _PTHREAD_ONCE_INIT       { 0, 0, 0, 0}

typedef struct pthread_key_t_ * pthread_key_t;

#endif /* defined(_POSIX_THREADS) || __POSIX_VISIBLE >= 199506 */

/* POSIX Barrier Types */

#if defined(_POSIX_BARRIERS)
typedef struct pthread_barrier_t_ * pthread_barrier_t;
typedef struct pthread_barrierattr_t_ * pthread_barrierattr_t;
#endif /* defined(_POSIX_BARRIERS) */

/* POSIX Spin Lock Types */

#if defined(_POSIX_SPIN_LOCKS)
typedef struct pthread_spinlock_t_ * pthread_spinlock_t;
#define PTHREAD_SPINLOCK_INITIALIZER ((pthread_spinlock_t) -1)
#endif /* defined(_POSIX_SPIN_LOCKS) */

/* POSIX Reader/Writer Lock Types */

#if defined(_POSIX_READER_WRITER_LOCKS)
typedef struct pthread_rwlock_t_ * pthread_rwlock_t;
typedef struct pthread_rwlockattr_t_ * pthread_rwlockattr_t;
#define _PTHREAD_RWLOCK_INITIALIZER ((pthread_rwlock_t) -1)

#endif /* defined(_POSIX_READER_WRITER_LOCKS) */

#endif /* ! _SYS__PTHREADTYPES_H_ */
