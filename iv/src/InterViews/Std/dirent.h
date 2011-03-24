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
 * <dirent.h> is the new interface to the old <sys/dir.h>
 */

#ifndef dirent_h

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined(opendir)
#define opendir dirent_h_opendir
#define readdir dirent_h_readdir
#define telldir dirent_h_telldir
#define seekdir dirent_h_seekdir
#define closedir dirent_h_closedir
#endif

#include "//usr/include/dirent.h"

#undef opendir
#undef readdir
#undef telldir
#undef seekdir
#undef closedir

#ifndef dirent_h
#define dirent_h
#endif

extern DIR* opendir(const char*);
extern struct dirent* readdir(DIR*);
extern long telldir(DIR*);
extern void seekdir(DIR*);
extern int closedir(DIR*);

#if defined(__cplusplus)
}
#endif

#endif
