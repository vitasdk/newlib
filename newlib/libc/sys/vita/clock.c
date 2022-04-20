/*

Copyright (C) 2019, Martin Larralde

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

#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <sys/times.h>
#include <reent.h>

#include <psp2/rtc.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>

#include "vitaerror.h"

clock_t clock()
{
	struct tms tim_s;
	clock_t res;

	if ((res = (clock_t) _times_r (_REENT, &tim_s)) != (clock_t) -1)
		res = (clock_t) (tim_s.tms_utime + tim_s.tms_stime +
				tim_s.tms_cutime + tim_s.tms_cstime);

	return res;
}

int clock_gettime(clockid_t clk_id, struct timespec *tp)
{
	time_t seconds;
	SceDateTime time;
	uint64_t t;

	if (!tp)
	{
		errno = EFAULT;
		return -1;
	}

	switch(clk_id) {
		case CLOCK_REALTIME:
			sceRtcGetCurrentClockLocalTime(&time);
			sceRtcGetTime_t(&time, &seconds);

			tp->tv_sec = seconds;
			tp->tv_nsec = time.microsecond * 1000;
			return 0;
			break;
		case CLOCK_MONOTONIC:
			t = sceKernelGetProcessTimeWide();
			tp->tv_sec = t / 1000000;
			tp->tv_nsec = (t % 1000000) * 1000;
			return 0;
			break;
		default:
			break;
	}

	errno = EINVAL;
	return -1;
}

int clock_settime(clockid_t clk_id, const struct timespec *tp)
{
	if (!tp)
	{
		errno = EFAULT;
		return -1;
	}

	errno = EPERM;
	return -1;
}

int clock_getres(clockid_t clk_id, struct timespec *res)
{
	if (!res)
	{
		errno = EFAULT;
		return -1;
	}

	switch(clk_id)
	{
		case CLOCK_REALTIME:
			res->tv_sec = 0;
			res->tv_nsec = 1000; // 1 microsecond
			return 0;
			break;
		case CLOCK_MONOTONIC:
			res->tv_sec = 0;
			res->tv_nsec = 1000; // 1 microsecond
			return 0;
			break;
		default:
			break;
	}

	errno = EINVAL;
	return -1;
}

int timer_create (clockid_t clock_id,
	struct sigevent *__restrict evp,
	timer_t *__restrict timerid)
{
	errno = ENOTSUP;
	return -1;
}

int timer_delete (timer_t timerid)
{
	errno = EINVAL; // since we can't create timers, any id would be invalid
	return -1;
}

int timer_settime (timer_t timerid, int flags,
	const struct itimerspec *__restrict value,
	struct itimerspec *__restrict ovalue)
{
	errno = EINVAL;
	return -1;
}

int timer_gettime (timer_t timerid, struct itimerspec *value)
{
	errno = EINVAL;
	return -1;
}

int timer_getoverrun (timer_t timerid)
{
	errno = EINVAL;
	return -1;
}

int nanosleep (const struct timespec *rqtp, struct timespec *rmtp)
{
	if (!rqtp)
	{
		errno = EFAULT;
		return -1;
	}

	if (rqtp->tv_sec < 0 || rqtp->tv_nsec < 0 || rqtp->tv_nsec > 999999999)
	{
		errno = EINVAL;
		return -1;
	}

	const uint32_t us = rqtp->tv_sec * 1000000 + (rqtp->tv_nsec+999) / 1000;

	sceKernelDelayThread(us);
	return 0;
}
