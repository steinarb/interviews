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
 * C++ interface to C library string functions
 */

#ifndef string_h
#define string_h

#if defined(__cplusplus)
extern "C" {
#endif

extern char* strcpy(char*, const char*);
extern char* strncpy(char*, const char*, int);
extern char* strcat(char*, const char*);
extern char* strncat(char*, const char*, int);

extern char* strchr(const char*, char);
extern char* strrchr(const char*, char);

extern int strcmp(const char*, const char*);
extern int strncmp(const char*, const char*, int);
extern int strlen(const char*);

extern int strspn(const char*, const char*);
extern int strcspn(const char*, const char*);

extern char* strpbrk(const char*, const char*);
extern char* strtok(char*, const char*);

extern char* index(const char*, char);
extern char* rindex(const char*, char);

#if defined(__cplusplus)
}
#endif

#endif
