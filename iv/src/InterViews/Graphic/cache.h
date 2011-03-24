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
 * Interface to associative Cache class.
 */

#ifndef cache_h
#define cache_h

#include <InterViews/defs.h>
#include <InterViews/Graphic/ref.h>

static const UID DIRTYBITMASK = 0x80000000;

class Cache {
    friend class ObjectMan;
public:
    Cache(int size);
    ~Cache();

    boolean IsDirty(Ref ptr);		/* we don't check to make sure */
    void Touch(Ref ptr);		/* ptr is really in memory, so */
    void Clean(Ref ptr);		/* you'd better make sure it is! */
    void TouchAll();
    void CleanAll();
    boolean Flush();			/* does a Save on valid pointers */

    void Set(Ref tag, Ref val);
    void Unset(Ref tag);
    Ref Get(Ref tag);
protected:
    boolean isDirty(Ref);
	/* the value of a pointer tag contains the dirty bit */
	/* (note that we don't do the same for the reverse mapping) */
    void touch(Ref&);
    void clean(Ref&);

    int hash(Ref tag);
	/* clean and dirty tags must hash to the same value */
    boolean tagsEqual(Ref, Ref);  
	/* a quick & dirty check; returns false if one of the tags is */
	/* is inMemory and the other isn't */
protected:
    int size;
    class CacheEntry** hashTable;
};

/*
 * inlines
 */

inline boolean Cache::isDirty (Ref r) { return (r.uid() & DIRTYBITMASK) != 0; }
inline void Cache::touch (Ref& r) { r._uid |= DIRTYBITMASK; }
inline void Cache::clean (Ref& r) { r._uid &= ~DIRTYBITMASK; }
inline int Cache::hash (Ref tag) { clean(tag); return tag.uid() % size; }

inline boolean Cache::tagsEqual (Ref tag1, Ref tag2) {
    if (tag1.inMemory() != tag2.inMemory()) {
	return false;
    } else if (!tag1.inMemory()) {
	tag1.resetClusterBit();
	tag2.resetClusterBit();
	return tag1.uid() == tag2.uid();
    } else {
	return tag1.refto == tag2.refto;
    }
}

#endif
