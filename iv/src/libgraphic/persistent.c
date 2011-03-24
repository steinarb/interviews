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
 * Persistent class implementation.  Persistent is the general persistent
 * object from which Graphic is derived.
 */

#include <InterViews/Graphic/objman.h>
#include <InterViews/Graphic/persistent.h>
#include <InterViews/Graphic/pfile.h>
#include <stdlib.h>

void Persistent::Warning (const char* msg) {
    fflush(stdout);
    fprintf(stderr, "warning: %s\n", msg);
}

void Persistent::Panic (const char* msg) {
    fflush(stdout);
    fprintf(stderr, "internal error: %s\n", msg);
    exit(2);
}

void Persistent::Fatal (const char* msg) {
    fflush(stdout);
    fprintf(stderr, "%s\n", msg);
    exit(2);
}

UID Persistent::getUID () {
    return TheManager->GetUID( this );
}

boolean Persistent::write (PFile*) {
    if (TheManager->Update(this)) {
	Clean();
	return true;
    } else {
	return false;
    }
}

boolean Persistent::read (PFile*) {
    return true;
}

boolean Persistent::readObjects (PFile*) {
    return true;
}

boolean Persistent::initialize () {
    return true;
}

ClassId Persistent::GetClassId () { 
    return PERSISTENT;
}

boolean Persistent::IsA (ClassId id) { 
    return PERSISTENT == id;
}

Persistent::Persistent () { } 

Persistent::~Persistent () {
    if (
	TheManager != nil && this != TheManager &&
	!TheManager->Invalidate(this)
    ) {
	Panic("couldn't invalidate object during destruction");
    }
}

Persistent* Persistent::GetCluster () { 
    return this;
}

boolean Persistent::Save () {
    return IsDirty() ? TheManager->Store(this) : true;
}

boolean Persistent::writeObjects (PFile* f) {
    return write(f);
}

boolean Persistent::IsDirty () {
    if (TheManager == nil) {
	return true;
    } else {
	return TheManager->ObjectIsDirty(this);
    }
}

void Persistent::Touch () {
    if (TheManager != nil) {
	TheManager->ObjectTouch(this);
    }
}

void Persistent::Clean () {
    if (TheManager != nil) {
	TheManager->ObjectClean(this);
    }
}
