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
 * Object space definition.
 */

#ifndef objectspace_h
#define objectspace_h

#include <InterViews/stub.h>

class Catalog;
class Messenger;
class ObjectTable;
class SpaceManager;

class ObjectSpace : public ObjectStub {
public:
    ObjectSpace(const char*);
    ~ObjectSpace();

    void StartServer(Connection* local, Connection* remote);

    void UsePath(const char*);
    Connection* Find(const char*, boolean wait = false);

    void Dispatch();

    void Message(Connection*, ObjectTag, int op, void*, int len);
    void Deliver(Connection*, ObjectTag, int op, void*, int);
    ObjectStub* Map(Connection*, ObjectTag);

    void AddChannel(int, ObjectStub*);
    void RemoveChannel(int);

    void Attach(int);
    void Detach(int);
protected:
    const char* name;
    Catalog* dictionary;
    ObjectTable* table;

    ObjectSpace();
private:
    struct MQueue {
	Messenger* head, * tail;
    };
    struct Stream {
	int channel, mask;
	ObjectStub* object;
	Stream* next;
    };

    Connection* local;
    Connection* remote;
    SpaceManager* manager;
    int channels;
    int maxchannel;
    MQueue active;
    MQueue inactive;
    Stream* streams;

    void Init();
    void Listen(Connection*);
    boolean Ready(int, Connection*);
    void CheckServer(int, Connection*);
    void CheckClients();
    void Add(Messenger*, MQueue&);
    void Remove(Messenger*, MQueue&);
    void CloseDown(Messenger*);
    virtual void AddClient(Connection*);
};

/*
 * Protocol definitions.
 */

static const int objectspace_Find = 1;
static const int objectspace_Clone = 2;
static const int objectspace_Destroy = 3;

struct objectspace_Msg {
    ObjectTag tag;
    char name[1];
    /* rest of name */
};

#endif
