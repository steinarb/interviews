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
 * Cache class implementation.  Cache is used by Persistent.
 */

#include <InterViews/Graphic/cache.h>

class CacheEntry {
public:
    Ref tag;
    Ref value;
    CacheEntry* next;

    CacheEntry() { next = nil; }
    CacheEntry(Ref t, Ref val) {
	this->tag = t; this->value = val; next = nil;
    }
    ~CacheEntry();
};

CacheEntry::~CacheEntry () {
    delete next;
}

Cache::Cache (int size) {
    hashTable = new CacheEntry* [size];
    this->size = size;
    for (CacheEntry** i = hashTable; i < &hashTable[size]; ++i) {
	*i = nil;
    }
}

Cache::~Cache () {
    for (CacheEntry** i = hashTable; i < &hashTable[size]; ++i) {
	delete *i;
    }
    delete hashTable;
}

boolean Cache::IsDirty (Ref ptr) {
    CacheEntry* cur = hashTable[hash(ptr)];

    while (cur != nil) {
	if (cur->tag.refto == ptr.refto) {
	    return isDirty(cur->value);
	} else {
	    cur = cur->next;
	}
    }
    return true;    // if not in cache, assume it's dirty
}

void Cache::Touch (Ref ptr) {
    CacheEntry* cur = hashTable[hash(ptr)];

    while (cur != nil) {
	if (cur->tag.refto == ptr.refto) {
	    touch(cur->value);
	    break;
	} else {
	    cur = cur->next;
	}
    }
}

void Cache::Clean (Ref ptr) {
    CacheEntry* cur = hashTable[hash(ptr)];

    while (cur != nil) {
	if (cur->tag.refto == ptr.refto) {
	    clean(cur->value);
	    break;
	} else {
	    cur = cur->next;
	}
    }
}

void Cache::TouchAll () {
    CacheEntry* cur;

    for (int i = 0; i < size; ++i) {
	cur = hashTable[i];
	while (cur != nil) {
	    if (!cur->value.inMemory()) {
		touch(cur->value);
	    }
	    cur = cur->next;
	}
    }
}

void Cache::CleanAll () {
    CacheEntry* cur;

    for (int i = 0; i < size; ++i) {
	cur = hashTable[i];
	while (cur != nil) {
	    if (!cur->value.inMemory()) {
		clean(cur->value);
	    }
	    cur = cur->next;
	}
    }
}

void Cache::Set (Ref tag, Ref val) {
    int h = hash(tag);
    CacheEntry* newEntry = new CacheEntry (tag, val);
    if (hashTable[h] != nil) {
	newEntry->next = hashTable[h];
    }
    hashTable[h] = newEntry;
}

void Cache::Unset (Ref tag) {
    int h = hash(tag);
    CacheEntry* cur = hashTable[h], *prev = nil;

    while (cur != nil) {
	if (tagsEqual(cur->tag, tag)) {
	    if (prev == nil) {
		hashTable[h] = cur->next;
	    } else {
		prev->next = cur->next;
	    }
	    cur->next = nil;
	    delete cur;
	    return;
	} else {
	    prev = cur;
	    cur = cur->next;
	}
    }
}

Ref Cache::Get (Ref tag) {
    CacheEntry* cur = hashTable[hash(tag)];

    while (cur != nil) {
	if (tagsEqual(cur->tag, tag)) {
	    return cur->value;
	} else {
	    cur = cur->next;
	}
    }
    return Ref();
}

boolean Cache::Flush () {
    CacheEntry** curBucket = hashTable;
    CacheEntry* i;
    boolean ok = true;
    boolean dirtFound;
    
    do {
	dirtFound = false;
	for (
	    curBucket = hashTable; 
	    curBucket < &hashTable[size] && ok; ++curBucket
	) {
	    if (*curBucket != nil) {
		i = *curBucket;
		while (i != nil && ok) {
		    if (i->tag.inMemory() && isDirty(i->value)) {
			ok = i->tag()->Save();
			dirtFound = true;
		    }
		    i = i->next;
		}
	    }
	}
    } while (dirtFound && ok);
    return ok;
}
