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
 * C++ interface to standard library functions
 */

#ifndef stdlib_h
#define stdlib_h

#if defined(__cplusplus)
extern "C" {
#endif

extern int atoi(const char*);
extern double atof(const char*);
extern long atol(const char*);
extern double strtod(const char*, char**);
extern long strtol(const char*, char**, int);

extern char* calloc(unsigned, unsigned);
extern char* malloc(unsigned);
extern char* realloc(char*, unsigned);
extern void free(char*);

extern char** environ;

extern int abort();
extern void exit(int);
extern char* getenv(const char*);

extern int rand();
extern void srand(unsigned);

extern char* getcwd(char*, int);
extern char* getwd(char*);

#if defined(__cplusplus)
}
#endif

#endif
