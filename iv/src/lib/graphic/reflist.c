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
 * RefList class implementation.  A RefList is a list of Refs, which are
 * references to Persistent objects.
 */

#include <InterViews/Graphic/reflist.h>

RefList* RefList::Find (Ref p) {
    register RefList* e;

    for (e = next; e != this; e = e->next) {
	if (*e == p) {
	    return e;
	}
    }
    return nil;
}

boolean RefList::Write (PFile* f) {
    RefList* i;
    int count = 0;

    for (i = next; i != this; i = i->next) {
        ++count;
    }
    boolean ok = f->Write(count);
    for (i = next; i != this && ok; i = i->next) {
	ok = ((Ref*) i)->Write(f);
    }
    return ok;
}

boolean RefList::Read (PFile* f) {
    int count;
    RefList* i;

    boolean ok = f->Read(count);
    for (--count; count >= 0 && ok; --count) {
	i = new RefList;
	ok = ((Ref*) i)->Read(f);
	Append(i);
    }
    return ok;
}

boolean RefList::WriteObjects (PFile* f) {
    RefList* i;
    boolean ok = true;

    for (i = next; i != this && ok; i = i->next) {
	ok = ((Ref*) i)->WriteObjects(f);
    }
    return ok;
}

boolean RefList::ReadObjects (PFile* f) {
    RefList* i;
    boolean ok = true;

    for (i = next; i != this && ok; i = i->next) {
	ok = ((Ref*) i)->ReadObjects(f);
    }
    return ok;
}

RefList* RefList::operator[] (int count) {
    RefList* pos = First();
    int i;

    for (i = 1; i < count && pos != End(); ++i) {
	pos = pos->Next();
    }
    if (i == count) {
	return pos;
    }
    return nil;
}	
