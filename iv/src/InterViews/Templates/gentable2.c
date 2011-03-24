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
 * Associative map between objects.
 */

#include "\name.h"

class \TableEntry {
    friend class \Table;

    \TableKey1 key1;
    \TableKey2 key2;
    \TableValue value;
    \TableEntry* chain;
};

\Table::\Table (int n) {
    register \TableEntry** e;

    size = 32;
    while (size < n) {
	size <<= 1;
    }
    first = new \TableEntry*[size];
    --size;
    last = &first[size];
    for (e = first; e <= last; e++) {
	*e = nil;
    }
}

\Table::~\Table () {
    delete first;
}

inline \TableEntry* \Table::Probe (\TableKey1 i1, \TableKey2 i2) {
    return first[((unsigned int)i1 + (unsigned int)i2) & size];
}

inline \TableEntry** \Table::ProbeAddr (\TableKey1 i1, \TableKey2 i2) {
    return &first[((unsigned int)i1 + (unsigned int)i2) & size];
}

void \Table::Insert (\TableKey1 k1, \TableKey2 k2, \TableValue v) {
    register \TableEntry* e;
    register \TableEntry** a;

    e = new \TableEntry;
    e->key1 = k1;
    e->key2 = k2;
    e->value = v;
    a = ProbeAddr(k1, k2);
    e->chain = *a;
    *a = e;
}

boolean \Table::Find (\TableValue& v, \TableKey1 k1, \TableKey2 k2) {
    register \TableEntry* e;

    for (e = Probe(k1, k2); e != nil; e = e->chain) {
	if (e->key1 == k1 && e->key2 == k2) {
	    v = e->value;
	    return true;
	}
    }
    return false;
}

void \Table::Remove (\TableKey1 k1, \TableKey2 k2) {
    register \TableEntry* e, * prev;
    \TableEntry** a;

    a = ProbeAddr(k1, k2);
    e = *a;
    if (e != nil) {
	if (e->key1 == k1 && e->key2 == k2) {
	    *a = e->chain;
	    delete e;
	} else {
	    do {
		prev = e;
		e = e->chain;
	    } while (e != nil && (e->key1 != k1 || e->key2 != k2));
	    if (e != nil) {
		prev->chain = e->chain;
		delete e;
	    }
	}
    }
}
