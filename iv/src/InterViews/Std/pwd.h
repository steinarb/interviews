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
 * C++ interface to password file entries
 */

#ifndef pwd_h
#define pwd_h

#if defined(__cplusplus)
extern "C" {
#endif

#define getpwent pwd_h_getpwent
#define getpwuid pwd_h_getpwuid
#define getpwnam pwd_h_getpwnam
#define setpwent pwd_h_setpwent
#define endpwent pwd_h_endpwent

#include "//usr/include/pwd.h"

#undef getpwent
#undef getpwuid
#undef getpwnam
#undef setpwent
#undef endpwent

/* just in case standard header didn't */
#ifndef pwd_h
#define pwd_h
#endif

extern struct passwd* getpwent();
extern struct passwd* getpwuid(int uid);
extern struct passwd* getpwnam(const char* name);

extern int setpwent();
extern int endpwent();

#if defined(__cplusplus)
}
#endif

#endif
