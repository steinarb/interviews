/*
 * Copyright (c) 1987, 1988, 1989 Stanford University
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

// $Header: istring.h,v 1.8 89/10/09 14:48:29 linton Exp $
// extends <string.h> to define strdup and strndup too.

#ifndef istring_h
#define istring_h

#include <string.h>

// strdup allocates and returns a duplicate of the given string.

inline char* strdup (const char* s) {
    char* dup = new char[strlen(s) + 1];
    strcpy(dup, s);
    return dup;
}

// strndup allocates and returns a duplicate of the first len
// characters of the given string.

inline char* strndup (const char* s, int len) {
    char* dup = new char[len + 1];
    strncpy(dup, s, len);
    dup[len] = '\0';
    return dup;
}

#endif
