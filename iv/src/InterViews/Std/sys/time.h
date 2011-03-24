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
 * C++ interface to Unix system time calls
 */

#ifndef sys_time_h

#if defined(__cplusplus)
extern "C" {
#endif

#define gettimeofday sys_time_h_gettimeofday
#define settimeofday sys_time_h_settimeofday

/*
 * Some systems (e.g., Ultrix) replicate <time.h> functions in <sys/time.h>.
 */

#define time time_h_time
#define ctime time_h_ctime
#define localtime time_h_localtime
#define gmtime time_h_gmtime
#define asctime time_h_asctime
#define tzset time_h_tzset

#include "//usr/include/sys/time.h"

#undef gettimeofday
#undef settimeofday

#undef time
#undef ctime
#undef localtime
#undef gmtime
#undef asctime
#undef tzset

/* just in case standard header didn't */
#ifndef sys_time_h
#define sys_time_h
#endif

/*
 * BSD-style get/set date and time
 */

extern int gettimeofday(struct timeval*, struct timezone*);
extern int settimeofday(struct timeval*, struct timezone*);

#if defined(__cplusplus)
}
#endif

#endif
