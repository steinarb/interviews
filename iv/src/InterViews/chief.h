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
 * A chief deputy multiplexes communcation with a remote object space
 * across a single connection.
 */

#ifndef chiefdeputy_h
#define chiefdeputy_h

#include <InterViews/defs.h>
#include <InterViews/tag.h>

class Connection;

class ChiefDeputy {
public:
    ChiefDeputy(Connection*);
    ~ChiefDeputy();

    Connection* GetServer() { return server; }
    ObjectTag Tag () { return 0; }
    void Sync();

    void Alloc(void*& dst, ObjectTag, int op, int len);
    int PackString(const char*, void*);
    int PackString(const char*, int, void*);
    void Msg(ObjectTag, int op);
    void StringMsg(ObjectTag, int op, const char*);
    void IntegerMsg(ObjectTag, int op, int val);

    int ReadReply(void* reply, int len);
    void GetReply(void* reply, int len);
    void GetString(const char*& str, int len);
private:
    Connection* server;
    char buffer[1024];
    int cur;
};

#endif
