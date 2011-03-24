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
 * C++ interface to C Internet address manipulation library
 */

#ifndef inet_h

#if defined(__cplusplus)
extern "C" {
#endif

#define inet_addr inet_h_inet_addr
#define inet_network inet_h_inet_network
#define inet_ntoa inet_h_inet_ntoa
#define inet_makeaddr inet_h_inet_makeaddr

#include "//usr/include/arpa/inet.h"

#undef inet_addr
#undef inet_network
#undef inet_ntoa
#undef inet_makeaddr

/* just in case standard header file didn't */
#ifndef inet_h
#define inet_h
#endif

extern unsigned long inet_addr(const char* cp);
extern unsigned long inet_network(const char* cp);
extern char* inet_ntoa(struct in_addr in);
extern struct in_addr inet_makeaddr(int net, int lna);
extern int inet_lnaof(struct in_addr in);
extern int inet_netof(struct in_addr in);

#if defined(__cplusplus)
}
#endif

#endif
