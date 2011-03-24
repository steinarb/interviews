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
 * C++ interface to C network data base library
 */

#ifndef netdb_h

#if defined(__cplusplus)
extern "C" {
#endif

#define gethostent netdb_h_gethostent
#define gethostbyname netdb_h_gethostbyname
#define gethostbyaddr netdb_h_gethostbyaddr
#define getnetent netdb_h_getnetent
#define getnetbyname netdb_h_getnetbyname
#define getnetbyaddr netdb_h_getnetbyaddr
#define getservent netdb_h_getservent
#define getservbyname netdb_h_getservbyname
#define getservbyport netdb_h_getservbyport
#define getprotoent netdb_h_getprotoent
#define getprotobyname netdb_h_getprotobyname
#define getprotobynumber netdb_h_getprotobynumber
#define getrpcent netdb_h_getrpcent
#define getrpcbyname netdb_h_getrpcbyname
#define getrpcbynumber netdb_h_getrpcbynumber
#define getsvcorder netdb_h_getsvcorder

#include "//usr/include/netdb.h"

#undef gethostent
#undef gethostbyname
#undef gethostbyaddr
#undef getnetent
#undef getnetbyname
#undef getnetbyaddr
#undef getservent
#undef getservbyname
#undef getservbyport
#undef getprotoent
#undef getprotobyname
#undef getprotobynumber
#undef getrpcent
#undef getrpcbyname
#undef getrpcbynumber
#undef getsvcorder

/* just in case standard header file didn't */
#ifndef netdb_h
#define netdb_h
#endif

extern void sethostent(int stayopen);
extern struct hostent* gethostent();
extern void endhostent();
extern struct hostent* gethostbyname(const char* name);
extern struct hostent* gethostbyaddr(const char* addr, int len, int type);

extern void setnetent(int stayopen);
extern struct netent* getnetent();
extern void endnetent();
extern struct netent* getnetbyname(const char* name);
extern struct netent* getnetbyaddr(long net, int type);

extern void setservent(int stayopen);
extern struct servent* getservent();
extern void endservent();
extern struct servent* getservbyname(const char* name, const char* proto);
extern struct servent* getservbyport(int port, const char* proto);

extern void setprotoent(int stayopen);
extern struct protoent* getprotoent();
extern void endprotoent();
extern struct protoent* getprotobyname(const char* name);
extern struct protoent* getprotobynumber(int proto);

extern void setrpcent(int stayopen);
extern struct rpcent* getrpcent();
extern void endrpcent();
extern struct rpcent* getrpcbyname(const char* name);
extern struct rpcent* getrpcbynumber(int number);

#if defined(__cplusplus)
}
#endif

#endif
