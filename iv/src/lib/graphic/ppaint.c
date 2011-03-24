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
 * Implementation of persistent paint objects used by Graphic.
 */

#include <InterViews/Graphic/grclasses.h>
#include <InterViews/Graphic/ppaint.h>
#include <InterViews/world.h>
#include <OS/memory.h>
#include <stdlib.h>
#include <string.h>

PColor* pblack;
PColor* pwhite;
PPattern* psolid;
PPattern* pclear;
PBrush* psingle;
PFont* pstdfont;

/*****************************************************************************/

boolean PColor::read(PFile* f) {
    int index;
    ColorIntensity r, g, b;
    boolean ok = 
	Persistent::read(f) && 
	f->Read(index) && f->Read(r) && f->Read(g) && f->Read(b);
    
    if (ok) {
	value = new Color(r, g, b);
	value->ref();
    }
    return ok;
}

boolean PColor::write(PFile* f) {
    ColorIntensity r, g, b;
    value->Intensities(r, g, b);
    return 
	Persistent::write(f) && f->Write(value->PixelValue()) &&
	f->Write(r) && f->Write(g) && f->Write(b);
}

ClassId PColor::GetClassId() { return PCOLOR; }
boolean PColor::IsA(ClassId id) { return PCOLOR == id || Persistent::IsA(id);}
PColor::PColor() { value = nil; }

PColor::PColor(ColorIntensity r, ColorIntensity g, ColorIntensity b) {
    value = new Color(r, g, b);
    value->ref();
}

PColor::PColor(const char* name) {
    value = Color::lookup(World::current()->display(), name);
    Resource::ref(value);
}

PColor::~PColor() {
    Resource::unref(value);
}

/*****************************************************************************/

boolean PPattern::read(PFile* f) {
    boolean ok = Persistent::read(f);

    if (ok) {
	ok = f->Read(data, patternHeight);
	if (ok) {
	    Resource::unref(value);
	    value = new Pattern(data);
	    value->ref();
	}
    }
    return ok;
}

boolean PPattern::write(PFile* f) {
    return Persistent::write(f) && f->Write(data, patternHeight);
}

ClassId PPattern::GetClassId() { return PPATTERN; }

boolean PPattern::IsA(ClassId id) { 
    return PPATTERN == id || Persistent::IsA(id);
}

PPattern::PPattern() { value = nil; }

PPattern::PPattern(int p[patternHeight]) {
    Memory::copy(p, this->data, sizeof(int)*patternHeight);
    value = new Pattern(p);
    value->ref();
}

PPattern::PPattern(int dither) {
    register int i;
    int r[4];

    for (i = 0; i < 4; i++) {
	r[i] = (dither & 0xf000) >> 12;
	r[i] |= r[i] << 4;
	r[i] |= r[i] << 8;
	dither <<= 4;
    }
    for (i = 0; i < patternHeight; i++) {
	data[i] = r[i%4];
    }
    value = new Pattern(data);
    value->ref();
}

PPattern::~PPattern() {
    Resource::unref(value);
}

/*****************************************************************************/

boolean PBrush::read(PFile* f) {
    int w;
    boolean ok = Persistent::read(f) && f->Read(w) && f->Read(p);
    
    if (ok) {
	if (w == NO_WIDTH) {
	    value = nil;
	} else {
	    value = new Brush(p, w);
	    value->ref();
	}
    }
    return ok;
}

boolean PBrush::write(PFile* f) {
    int width = (value == nil) ? NO_WIDTH : value->Width();
    return Persistent::write(f) && f->Write(width) && f->Write(p);
}

ClassId PBrush::GetClassId() { return PBRUSH; }
boolean PBrush::IsA(ClassId id) { return PBRUSH == id || Persistent::IsA(id);}
PBrush::PBrush() { value = nil; }

PBrush::PBrush(int p, int w) {
    this->p = p;
    if (w == NO_WIDTH) {
	value = nil;
    } else {
	value = new Brush(p, w);
	value->ref();
    }
}

PBrush::~PBrush() {
    Resource::unref(value);
}

/*****************************************************************************/

boolean PFont::read(PFile* f) {
    int count;
    boolean ok = Persistent::read(f) && f->Read(count);
    
    if (ok) {
	delete name;
	this->count = count;
	if (count == 0) {
	    name = nil;
	    Resource::unref(value);
	    value = World::current()->font();
	    value->ref();
	} else {
	    name = new char[count+1];
	    ok = f->Read(name);
	    if (ok) {
		name[count] = '\0';
		Resource::unref(value);
		value = Font::lookup(name);
		Resource::ref(value);
	    }
	}
    }
    return ok;
}

boolean PFont::write(PFile* f) {
    boolean ok = Persistent::write(f) && f->Write(count);
    if (ok && count > 0) {
	ok = f->Write(name);
    }
    return ok;
}

ClassId PFont::GetClassId() { return PFONT; }
boolean PFont::IsA(ClassId id) { return PFONT == id || Persistent::IsA(id); }

PFont::PFont() {
    name = nil;
    value = nil;
}

PFont::PFont(const char* name) {
    World* w = World::current();
    if (name == nil) {
	count = 0;
	value = w->font();
	value->ref();
	this->name = nil;
    } else {
	count = strlen(name) + 1;
	this->name = new char [count];
	strcpy(this->name, name);
	value = Font::lookup(this->name);
	Resource::ref(value);
    }
}

PFont::PFont(const char* name, int count) {
    this->count = count + 1;
    this->name = new char [count + 1];
    strncpy(this->name, name, count);
    this->name[count] = '\0';
    value = Font::lookup(this->name);
    Resource::ref(value);
}

PFont::~PFont() {
    if (count > 0) {
	delete name;
	Resource::unref(value);
    }
}

int PFont::Index(const char* s, int offset, boolean between) {
    return value->Index(s, offset, between);
}

int PFont::Index(const char* s, int len, int offset, boolean between) {
    return value->Index(s, len, offset, between);
}

/*****************************************************************************/

static int spat[] = {
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
};

static int cpat[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void InitPPaint () {
    pblack = new PColor("black");
    pwhite = new PColor("white");
    pstdfont = new PFont(nil);
    psolid = new PPattern(spat);
    pclear = new PPattern(cpat);
    psingle = new PBrush(0xffff);
}
