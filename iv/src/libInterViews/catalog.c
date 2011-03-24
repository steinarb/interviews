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
 * A catalog maps names to objects.
 */

#include <InterViews/catalog.h>
#include <bstring.h>
#include <string.h>

Catalog::Catalog (int n) {
    nelements = 128;
    while (nelements < n) {
	nelements <<= 1;
    }
    first = new CatalogEntry*[nelements];
    bzero(first, nelements * sizeof(CatalogEntry*));
    --nelements;
    last = &first[nelements];
}

Catalog::~Catalog () {
    register CatalogEntry** i;
    register CatalogEntry* e;
    register CatalogEntry* next;

    for (i = first; i <= last; i++) {
	for (e = *i; e != nil; e = next) {
	    next = e->chain;
	    delete e->name;
	    Unref(e->obj);
	    delete e;
	}
    }
    delete first;
}

/*
 * Enter a new object into a catalog.
 */

void Catalog::Register (const char* name, ObjectStub* s) {
    register unsigned h;
    int len;
    register CatalogEntry* e;

    h = Hash(name, len);
    e = new CatalogEntry;
    e->name = new char[len+1];
    e->len = len;
    strcpy(e->name, name);
    e->obj = s;
    e->chain = first[h];
    first[h] = e;
}

void Catalog::UnRegister (const char* name) {
    register unsigned h;
    int len;
    register CatalogEntry* e;
    register CatalogEntry* prev;

    h = Hash(name, len);
    e = first[h];
    prev = nil;
    while (e != nil && (e->len != len || strcmp(e->name, name) != 0)) {
	prev = e;
	e = e->chain;
    }
    if (e != nil) {
	if (prev == nil) {
	    first[h] = e->chain;
	} else {
	    prev->chain = e->chain;
	}
	delete e->name;
	delete e;
    }
}

boolean Catalog::Find (ObjectStub*& s, const char* name) {
    int len;
    register CatalogEntry* e;

    for (e = first[Hash(name, len)]; e != nil; e = e->chain) {
	if (e->len == len && strcmp(e->name, name) == 0) {
	    s = e->obj;
	    return true;
	}
    }
    return false;
}

/*
 * Return a hash value for a string name and set the length of the name
 * as a side effect.
 */

unsigned Catalog::Hash (const char *name, int& len) {
    register const char *c;
    register unsigned h;

    h = 0;
    c = name;
    while (*c != '\0') {
	h = (h << 1) ^ ((int) (*c));
	++c;
    }
    len = (c - name);
    return h & nelements;
}
