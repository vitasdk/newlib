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

#include <psp2/rtc.h>

#include "vitaerror.h"

#define TRY(x) {rid=x; if (rid < 0) {errno = __vita_sce_errno_to_errno(rid, ERROR_GENERIC); return NULL;}}


struct tm *localtime_r(const time_t *timep, struct tm *result)
{
	SceDateTime dt;
	SceRtcTick tick;
	time_t localtime;
	int rid;

	TRY(sceRtcConvertTime_tToDateTime(*timep, &dt));
	TRY(sceRtcConvertDateTimeToTick(&dt, &tick));
	TRY(sceRtcConvertUtcToLocalTime(&tick, &tick));
	TRY(sceRtcConvertTickToDateTime(&tick, &dt));
	TRY(sceRtcConvertDateTimeToTime_t(&dt, &localtime));

	return gmtime_r(&localtime, result);
}
