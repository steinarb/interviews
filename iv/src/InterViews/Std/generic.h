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
 * Generic C++ macros
 */

#ifndef generic_h
#define generic_h

/*
 * Concatenate strings
 */

#if defined(__STDC__)
#define name2(a,b) name2_concat(a,b)
#define name2_concat(a,b) a##b
#define name3(a,b,c) name3_concat(a,b,c)
#define name3_concat(a,b,c) a##b##c
#define name4(a,b,c,c) name4_concat(a,b,c,d)
#define name4_concat(a,b,c,d) a##b##c##d
#else
#define name2(a,b) a/**/b
#define name3(a,b,c) a/**/b
#define name4(a,b,c,d) a/**/b
#endif

/*
 * Instantiate template names
 */

#define declare(template,param) name2(template,declare)(param)
#define implement(template,param) name2(template,implement)(param)

#define declare2(template,p1,p2) name2(template,declare2)(p1,p2)
#define implement2(template,p1,p2) name2(template,implement2)(p1,p2)

#endif
