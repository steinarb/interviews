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
 * C++ interface to C library regular expression functions
 */

#ifndef regexpr_h
#define regexpr_h

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(hpux) || defined(SYSTEM_FIVE) || defined(SYSV)

/*
 * System V regular expression routines
 */

extern char* regcmp(const char* s1, ...);
extern char* regex(const char* re, const char* subj, char* ret0, ...);
extern char* __loc1;
extern void free(void*);

static char* re_exp = 0;

inline const char* re_comp(const char* pattern) {
    if (re_exp != 0) {
	free(re_exp);
    }
    re_exp = regcmp(pattern, 0);
    return re_exp;
}

inline int re_exec(const char* re) {
    return regex(re_exp, re, 0) != 0;
}

#else

/*
 * BSD library routines
 */

extern const char* re_comp(const char* pattern);
extern int re_exec(const char* string);

#endif

#if defined(__cplusplus)
}
#endif

#endif
