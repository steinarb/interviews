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
 * Map strings to unique pointers.
 */

#include <InterViews/strpool.h>
#include <InterViews/strtable.h>
#include <string.h>

static const int nolength = -1;

static StringPool* pool;

StringTable::StringTable (int n) {
    register StringId** s;

    size = 32;
    while (size < n) {
	size <<= 1;
    }
    first = new StringId*[size];
    --size;
    last = &first[size];
    for (s = first; s <= last; s++) {
	*s = nil;
    }
    if (pool == nil) {
	pool = new StringPool;
    }
}

StringTable::~StringTable () {
    delete first;
}

inline StringId** StringTable::Probe (const char* str, int& len) {
    return &first[Hash(str, len) & size];
}

StringId* StringTable::Id (const char* str) {
    return Id(str, nolength);
}

StringId* StringTable::Id (const char* str, int len) {
    register StringId* s;
    register StringId** a;
    register int n;
    int slen;
    char* newstr;

    slen = len;
    a = Probe(str, slen);
    n = slen;
    for (s = *a; s != nil; s = s->chain) {
	if (s->len == n && strncmp(s->str, str, n) == 0) {
	    return s;
	}
    }
    s = new StringId;
    newstr = pool->Append(str, n + 1);
    newstr[n] = '\0';
    s->str = newstr;
    s->len = n;
    s->chain = *a;
    *a = s;
    return s;
}

void StringTable::Remove (const char* str) {
    Remove(str, nolength);
}

void StringTable::Remove (const char* str, int len) {
    register StringId* s, * prev;
    StringId** a;

    a = Probe(str, len);
    s = *a;
    if (s != nil) {
	if (s->len == len && strcmp(s->str, str) == 0) {
	    *a = s->chain;
	    delete s;
	} else {
	    for (prev = s; ; s = s->chain) {
		if (s == nil) {
		    break;
		}
		if (s->len == len && strcmp(s->str, str) == 0) {
		    prev->chain = s->chain;
		    delete s;
		    break;
		}
	    }
	}
    }
}

unsigned int StringTable::Hash (const char* str, int& len) {
    register const char* p;
    register unsigned int v;
    register unsigned int t;

    v = 0;
    if (len == nolength) {
	for (p = str; *p != '\0'; p++) {
	    v += v;
	    v ^= *p;
	}
	len = p - str;
    } else {
	const char* q = &str[len];
	for (p = str; p < q; p++) {
	    v += v;
	    v ^= *p;
	}
    }
    t = v >> 10;
    t ^= t >> 10;
    v ^= t;
    return v;
}
