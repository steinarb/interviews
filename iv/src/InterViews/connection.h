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

/*
 * Connections provide an interface to
 * the OS IPC mechanism.
 */

#ifndef connection_h
#define connection_h

#include <InterViews/defs.h>

class Connection {
public:
    Connection();
    Connection(int);
    ~Connection();

    void CreateService(const char* host, int port);
    void CreateLocalService(const char*);
    boolean OpenService(const char* host, int port);
    boolean OpenLocalService(const char*);
    Connection* AcceptClient();

    int Pending();
    int Read(void*, int);
    int Write(const void*, int);
    int WritePad(const void*, int, int);
    int Descriptor () { return fd; }
    void Close();
protected:
    int fd;
    int domain;
    const char* name;
    int port;

    boolean MakeSocket (struct sockaddr_in&);
    int MakeLocalSocket (struct sockaddr_un&);
};

inline int WordAlign (int n) {
    return (n + sizeof(int) - 1) & ~(sizeof(int) - 1);
}

#endif
