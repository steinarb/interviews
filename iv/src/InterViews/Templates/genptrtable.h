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
 * Object association table.
 */

#ifndef \Table_h
#define \Table_h

#include <InterViews/table.h>

class \TableEntry;

class \Table : public Table {
public:
    \Table(int);
    void Insert(\TableKey, \TableValue);
    boolean Find(\TableValue&, \TableKey);
    void Remove(\TableKey);
};

inline \Table::\Table (int n) : (n) {}

inline void \Table::Insert (\TableKey k, \TableValue v) {
    Table::Insert((void*)k, (void*)v);
}

inline boolean \Table::Find (\TableValue& v, \TableKey k) {
    void* vv;

    boolean b = Table::Find(vv, (void*)k);
    if (b) {
	v = (\TableValue)vv;
    }
    return b;
}

inline void \Table::Remove (\TableKey k) {
    Table::Remove((void*)k);
}

#endif
