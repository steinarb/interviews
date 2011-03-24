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
 * Object tables are implemented by hashing resolved by chaining.
 */

#include <InterViews/tagtable.h>
#include <bstring.h>

/*
 * Create a new table of (at least) a given size.
 *
 * To make hashing faster, the size is rounded up to the next power of 2 and
 * then decremented so it can be used as a mask.
 */

ObjectTable::ObjectTable (int n) {
    nelements = 128;
    while (nelements < n) {
	nelements <<= 1;
    }
    first = new ObjectTableEntry*[nelements];
    bzero(first, nelements * sizeof(ObjectTableEntry*));
    --nelements;
    last = &first[nelements];
}

ObjectTable::~ObjectTable () {
    delete first;
}

inline unsigned ObjectTable::Hash (Connection* c, ObjectTag t) {
    return ((((unsigned) c) >> 2) ^ (t >> 2)) & nelements;
}

inline ObjectTableEntry* ObjectTable::Start (Connection* c, ObjectTag t) {
    return first[Hash(c, t)];
}

inline ObjectTableEntry** ObjectTable::StartAddr (Connection* c, ObjectTag t) {
    return &first[Hash(c, t)];
}

inline boolean ObjectTableEntry::Match (Connection* c, ObjectTag t) {
    return client == c && id == t;
}

/*
 * Add an object into the table.
 */

void ObjectTable::Add (Connection* c, ObjectTag t, ObjectStub* s) {
    register ObjectTableEntry* e;
    register ObjectTableEntry** a;

    e = new ObjectTableEntry;
    e->client = c;
    e->id = t;
    e->ref = s;
    a = StartAddr(c, t);
    e->chain = *a;
    *a = e;
}

/*
 * Find an object in the table.
 * If found return a pointer to the object, otherwise return nil.
 */

ObjectStub* ObjectTable::Find (Connection* c, ObjectTag t) {
    register ObjectTableEntry* e;

    for (e = Start(c, t); e != nil; e = e->chain) {
	if (e->Match(c, t)) {
	    return e->ref;
	}
    }
    return nil;
}

/*
 * Remove an object from the table.
 */

void ObjectTable::Remove (Connection* c, ObjectTag t) {
    register ObjectTableEntry* e;
    register ObjectTableEntry* prev;
    register ObjectTableEntry** a;

    a = StartAddr(c, t);
    e = *a;
    if (e->Match(c, t)) {
	*a = e->chain;
    } else {
	do {
	    prev = e;
	    e = e->chain;
	} while (e != nil && !e->Match(c, t));
	if (e != nil) {
	    prev->chain = e->chain;
	    delete e;
	}
    }
}

/*
 * Remove all the objects associated with a given connection.
 */

void ObjectTable::RemoveAll (Connection* c) {
    register ObjectTableEntry** i;
    register ObjectTableEntry* e;
    register ObjectTableEntry* next;
    register ObjectTableEntry* prev;

    for (i = first; i <= last; i++) {
	prev = nil;
	for (e = *i; e != nil; e = next) {
	    next = e->chain;
	    if (e->client == c) {
		if (prev == nil) {
		    *i = e->chain;
		} else {
		    prev->chain = e->chain;
		}
		Unref(e->ref);
		delete e;
	    }
	}
    }
}
