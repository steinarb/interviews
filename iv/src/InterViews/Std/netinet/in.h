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
 * C++ interface to internet definitions.
 */

#ifndef netinet_in_h

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(sgi)
#include "//usr/include/bsd/netinet/in.h"
#else
#include "//usr/include/netinet/in.h"
#endif

/* just in case standard header didn't */
#ifndef netinet_in_h
#define netinet_in_h
#endif

/*
 * Can't use the library names because
 * C header might use macros or prototype-less functions.
 */

#if defined(htons)

/*
 * The operations are defined as macros, so we can
 * define the functions using the macros.
 */

inline u_short short_host_to_net (u_short x) {
    return htons(x);
}

inline u_long long_host_to_net (u_long x) {
    return htonl(x);
}

inline u_short short_net_to_host (u_short x) {
    return ntohs(x);
}

inline u_long long_net_to_host (u_long x) {
    return ntohl(x);
}

#else

#if defined(vax)
/*
 * VAX Ultrix doesn't define anything yet, so add it here.
 * When Ultrix defines them, remove this #if - #endif code.
 */

extern u_short htons(), ntohs();
extern u_long htonl(), ntohl();

#endif

/*
 * The operations are defined as functions, possibly without prototypes.
 * So, we cast pointers to the functions and call indirect.
 *
 * Yuck!!
 */

typedef u_short (*_nsfunc)(u_short);
typedef u_long (*_nlfunc)(u_long);

inline u_short short_host_to_net (u_short x) {
    return (*((_nsfunc)&htons))(x);
}

inline u_long long_host_to_net (u_long x) {
    return (*((_nlfunc)&htonl))(x);
}

inline u_short short_net_to_host (u_short x) {
    return (*((_nsfunc)&ntohs))(x);
}

inline u_long long_net_to_host (u_long x) {
    return (*((_nlfunc)&ntohl))(x);
}

#endif

#if defined(__cplusplus)
}
#endif

#endif
