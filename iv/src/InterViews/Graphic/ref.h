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
 * Interface to Ref (persistent object reference) base class.
 */

#ifndef ref_h
#define ref_h

#include <stdio.h>
#include <InterViews/Graphic/pfile.h>
#include <InterViews/Graphic/objman.h>
#include <InterViews/Graphic/persistent.h>

static const UID CLUSTERBITMASK = 0x80000000;
static const UID INMEMORYBITMASK = 0x1;

class Ref {
    friend class Cache;
    friend class ObjectMan;
public:
    Ref();
    Ref(UID);
    Ref(Persistent*);
    
    UID uid();
    Persistent* operator()();
    boolean Valid();		    /* true if ref is non-nil/non-INVALIDUID */

    boolean operator==(Ref r);
    boolean operator!=(Ref r);

    boolean Write(PFile*);	    /* write uid + cluster bit */
    boolean Read(PFile*);	    /* read uid + cluster bit */
    boolean WriteObjects(PFile*);   /* write object if not head of a cluster */
    boolean ReadObjects(PFile*);    /* read object if not head of a cluster */
protected:
    void uid (UID);                 /* set uid */

    void Warning(const char*);
    void Panic(const char*, int);
    
    boolean inMemory();
    boolean isCluster();            /* checks cluster bit (msb UID); it's */
				    /* set if object is a head of a cluster */
				    /* (doesn't check if ref is inMemory!) */
    void setClusterBit();
    void resetClusterBit();
    UID getUID();	            /* removes cluster bit */

    Persistent* ref();		/* returns object (possibly faulting it in) */
    Persistent* refObjects();	    /* seekless ref() for faulting in  */
				    /* consecutive objects */
    void unref();		    /* converts ref to uid w/cluster bit */
protected:
    union {			/* distinguished in that a UID has lsb == 1 */
	Persistent* refto;
	UID _uid;
    };
};

/*
 * inlines
 */

inline Ref::Ref (Persistent* obj) {
    refto = (obj == nil) ? (Persistent*) INVALIDUID : obj;
}

inline UID Ref::uid () { return _uid; }
inline void Ref::uid (UID u) { _uid = u; }

inline boolean Ref::inMemory () {
    return (uid() & INMEMORYBITMASK) == 0;    /* test inMemory bit */
}

inline boolean Ref::isCluster () { return (uid() & CLUSTERBITMASK) != 0; }
inline void Ref::setClusterBit () { _uid |= CLUSTERBITMASK; }
inline void Ref::resetClusterBit () { _uid &= ~CLUSTERBITMASK; }

inline Persistent* Ref::ref () {
    if (uid() == INVALIDUID) {
	return (Persistent*) nil;
    } else if (refto != nil && !inMemory() && !TheManager->Find( this ) ) {
	Panic( "unable to find object ", uid() );
	return (Persistent*) nil;	// so the compiler won't squawk
    } else {
	return refto;
    }
}

inline Persistent* Ref::operator() () { return ref(); }
inline boolean Ref::Valid () { return uid() != INVALIDUID && refto != nil; }

inline boolean Ref::operator!= (Ref r) { return ! (*this == r); }

inline boolean Ref::Write (PFile* f) {
    unref();
    return f->Write((int)uid());
}

inline boolean Ref::WriteObjects (PFile* f) {
    if (isCluster()) {
	return true;	    /* if it's the head of a cluster, we're not */
			    /* responsible for writing it out */
    } else if ( inMemory() || TheManager->IsCached(this) ) {
	return refto->writeObjects(f);
    } else if (uid() != INVALIDUID) {
	Warning("object within a cluster not in memory prior to write");
	return false;
    } else {
	return true;
    }
}

inline boolean Ref::ReadObjects (PFile*) {
    if (!isCluster()) {
	(void*) refObjects();
    }
    return true;
}

#endif
