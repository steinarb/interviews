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
 * C++ interface to Unix BSD sockets
 *
 * This file avoids conflicts with 2.0 <sys/socket.h> function definitions
 * that we define in <os/ipc.h>.
 */

#ifndef sys_socket_h

#if defined(__cplusplus)
extern "C" {
#endif

/* avoid these defs for HP-UX */
#define accept              c_accept
#define bind                c_bind
#define connect             c_connect
#define endhostent          c_endhostent
#define endnetent           c_endnetent
#define endprotoent         c_endprotoent
#define endservent          c_endservent
#define gethostbyaddr       c_gethostbyaddr
#define gethostbyname       c_gethostbyname
#define getnetbyaddr        c_getnetbyaddr
#define getnetbyname        c_getnetbyname
#define getprotobyname      c_getprotobyname
#define getprotobynumber    c_getprotobynumber
#define getservbyname       c_getservbyname
#define getservbyport       c_getservbyport
#define getsockname         c_getsockname
#define listen              c_listen
#define sethostent          c_sethostent
#define setnetent           c_setnetent
#define setprotoent         c_setprotoent
#define setservent          c_setservent
#define socket              c_socket

#include "//usr/include/sys/socket.h"

#undef accept
#undef bind
#undef connect
#undef endhostent
#undef endnetent
#undef endprotoent
#undef endservent
#undef gethostbyaddr
#undef gethostbyname
#undef getnetbyaddr
#undef getnetbyname
#undef getprotobyname
#undef getprotobynumber
#undef getservbyname
#undef getservbyport
#undef getsockname
#undef listen
#undef sethostent
#undef setnetent
#undef setprotoent
#undef setservent
#undef socket

/* just in case standard header didn't */
#ifndef sys_socket_h
#define sys_socket_h
#endif

#if defined(__cplusplus)
}
#endif

#endif
