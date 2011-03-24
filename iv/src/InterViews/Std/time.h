/*
 * Copyright (c) 1989 Stanford University
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Stanford not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Stanford makes no representations about
 * the suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * STANFORD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL STANFORD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * C++ interface to system time routines
 */

#ifndef time_h

#if defined(__cplusplus)
extern "C" {
#endif

#define time time_h_time
#define ctime time_h_ctime
#define localtime time_h_localtime
#define gmtime time_h_gmtime
#define asctime time_h_asctime
#define tzset time_h_tzset

#include "//usr/include/time.h"

#undef time
#undef ctime
#undef localtime
#undef gmtime
#undef asctime
#undef tzset

/* just in case standard header didn't */
#ifndef time_h
#define time_h
#endif

extern long time(long*);
extern char* ctime(long*);
extern struct tm* localtime(long*);
extern struct tm* gmtime(long*);
extern char* asctime(struct tm*);

/*
 * Can't use the timezone name here because it may have been used in <time.h>.
 * Can't redefine it like other symbols because we need the timezone struct.
 */

#if defined(SYSTEM_FIVE) || defined(SYSV)

extern long time_zone;
extern int daylight;
extern char* tzname[2];
extern void tzset();

#else

extern char* time_zone(int zone, int dst);

#endif

#if defined(__cplusplus)
}
#endif

#endif
