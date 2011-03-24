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
 * Ref class implementation.  A Ref is a reference to a Persistent object.
 */

#include <InterViews/Graphic/ref.h>
#include <stdlib.h>

Ref::Ref () { uid(INVALIDUID); }
Ref::Ref (UID u) { uid(u); }

UID Ref::getUID () {
    return inMemory() &&
	(refto != nil) ? refto->getUID() : (uid() & ~CLUSTERBITMASK);
}

void Ref::Warning (const char* msg) {
    fflush(stdout);
    fprintf(stderr, "warning: %s\n", msg);
}

void Ref::Panic (const char* msg, int n) {
    fflush(stdout);
    fprintf(stderr, msg, n);
    fprintf(stderr, "\n");
    exit(2);
}

Persistent* Ref::refObjects () {
    if (uid() == INVALIDUID) {
	return (Persistent*) nil;
    } else if (refto != nil && !inMemory() && !TheManager->Retrieve( this ) ) {
	Panic( "unable to find object ", uid() );
	return (Persistent*) nil;	// so the compiler won't squawk
    } else {
	return refto;
    }
}

void Ref::unref () {
    if (inMemory() && refto != nil) {
	Persistent* obj = refto;
	uid(refto->getUID());
	
	if (obj->GetCluster() == obj) { // obj can't be nil here
	    setClusterBit();		// it is the head of a cluster
	} else {
	    resetClusterBit();
	    	// it's not the head of a cluster. Note that if GetCluster
		// returns nil (i.e., the object is part of whatever
		// cluster is currently being read/written), it's not
		// considered the head of a cluster
	}
    } else if (refto == nil) {
	uid(INVALIDUID);
    }
}

boolean Ref::Read (PFile* f) {
#if defined(__cplusplus)
    int u;
#else
    UID u;
#endif
    boolean b = f->Read(u);

    uid(u);
    return b;
}

boolean Ref::operator== (Ref r) {
    if (r.inMemory() && inMemory()) {
	return r.refto == refto;
    } else if (!r.inMemory() && !inMemory()) {
	return (r.uid() & ~CLUSTERBITMASK) == (uid() & ~CLUSTERBITMASK) ;
    } else if (!r.Valid() && !Valid()) {
	return true;
    } else if (r.Valid() != Valid()) {
	return false;
    } else {
	return r.getUID() == getUID();
    }
}
