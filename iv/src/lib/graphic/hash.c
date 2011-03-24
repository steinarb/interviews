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
 * HashTable implementation.
 */

#include <InterViews/defs.h>
#include <InterViews/painter.h>
#include <InterViews/Graphic/base.h>
#include <InterViews/Graphic/hash.h>

class Entry {
public:
    void* tag;
    void* value;
    Entry* next;
    Entry(void*, void*);
};

Entry::Entry(void* t, void* v) {
    tag = t;
    value = v;
    next = nil;
}

HashTable::HashTable(int c) {
    count = c;
    typedef Entry* ep;
    entries = new ep[ count ];
    for (int i = 0; i<count; ++i) {
	entries[i] = nil;
    }
}

HashTable::~HashTable() {
    for (int i = 0; i<count; ++i) {
	Entry* entry = entries[i];
	while (entry != nil) {
	    Entry* doomed = entry;
	    entry = entry->next;
	    delete doomed;
	}
    }
    delete entries;
}

boolean HashTable::Match(void* t1, void* t2) {
    return t1 == t2;
}

int HashTable::Hash(void* tag) {
    return ((unsigned int)tag) % count;
}

void HashTable::Insert(void* tag, void* value) {
    int hash = Hash(tag);

    Entry* newEntry = new Entry(tag, value);
    newEntry->next = entries[hash];
    entries[hash] = newEntry;
}

void HashTable::Remove(void* tag) {
    int hash = Hash(tag);
    Entry* entry = entries[hash];
    Entry* prev = nil;

    while (entry != nil) {
	if (Match(tag, entry->tag) ) {
	    break;
	} else {
	    prev = entry;
	    entry = entry->next;
	}
    }
    if (entry != nil) {
	if (prev != nil) {
	    prev->next = entry->next;
	} else {
	    entries[hash] = entry->next;
	}
	delete entry;
    }
}

void* HashTable::Find(void* tag) {
    int hash = Hash(tag);
    Entry* entry = entries[hash];

    while (entry != nil) {
	if (Match(tag, entry->tag) ) {
	    break;
	} else {
	    entry = entry->next;
	}
    }
    if (entry != nil) {
	return entry->value;
    } else {
	return nil;
    }
}

GraphicToPainter::GraphicToPainter(int count) : HashTable(count) {
    canvas = nil;
    box = new BoxObj;
}

GraphicToPainter::~GraphicToPainter() {
    delete box;
}

boolean GraphicToPainter::Match(void* a, void* b) {
    Graphic* g = (Graphic*) a;
    Painter* p = (Painter*) b;
    boolean fill = g->BgFilled();
    PColor* pfg = g->GetFgColor();
    PColor* pbg = g->GetBgColor();
    PPattern* pat = g->GetPattern();
    PBrush* br = g->GetBrush();
    PFont* font = g->GetFont();

    return
	(fill == UNDEF || fill == p->BgFilled()) &&
	(pfg == nil || *pfg == p->GetFgColor()) &&
	(pbg == nil || *pbg == p->GetBgColor()) &&
	(pat == nil || *pat == p->GetPattern()) &&
	(br == nil || *br == p->GetBrush()) &&
	(font == nil || *font == p->GetFont());
}

int GraphicToPainter::Hash(void* g) {
    Graphic* gr = (Graphic*) g;
    unsigned int n = 0;
    boolean fill = gr->BgFilled();
    PColor* pfg = gr->GetFgColor();
    PColor* pbg = gr->GetBgColor();
    PPattern* pat = gr->GetPattern();
    PBrush* br = gr->GetBrush();
    PFont* font = gr->GetFont();
    
    if (fill != UNDEF) n += (unsigned int) fill;
    if (pfg != nil) n += (unsigned int) (const Color*) *pfg;
    if (pbg != nil) n += (unsigned int) (const Color*) *pbg;
    if (pat != nil) n += (unsigned int) (const Pattern*) *pat;
    if (br != nil) n += (unsigned int) (const Brush*) *br;
    if (font != nil) n += (unsigned int) (const Font*) *font;
    n >> 3;
    return n % count;
}

void GraphicToPainter::Insert(Graphic* g, Painter*& p) {
    int hash = Hash(g);
    boolean fill = g->BgFilled();
    PColor* pfg = g->GetFgColor();
    PColor* pbg = g->GetBgColor();
    PPattern* pat = g->GetPattern();
    PBrush* br = g->GetBrush();
    PFont* font = g->GetFont();
    const Color* fg, *bg;

    p = new Painter;
    p->Reference();
    if (fill != UNDEF) p->FillBg(fill);
    if (pfg == nil) fg = nil; else fg = *pfg;
    if (pbg == nil) bg = nil; else bg = *pbg;
    p->SetColors(fg, bg);
    if (pat != nil) p->SetPattern(*pat);
    if (br != nil) p->SetBrush(*br);
    if (font != nil) p->SetFont(*font);

    if (canvas != nil) {
	p->Clip(canvas, box->left, box->bottom, box->right, box->top);
    }	

    Entry* newEntry = new Entry(p, p);
    newEntry->next = entries[hash];
    entries[hash] = newEntry;
}

Painter* GraphicToPainter::Find(Graphic* g) {
    Painter* p = (Painter*) HashTable::Find(g);

    if (p == nil) {
	Insert(g, p);
    }
    return p;
}

void GraphicToPainter::Clip(Canvas* c, Coord l, Coord b, Coord r, Coord t) {
    Entry* entry;

    canvas = c;
    box->left = l; box->bottom = b; box->right = r; box->top = t;
    
    for (int i = 0; i < count; ++i) {
	for (entry = entries[i]; entry != nil; entry = entry->next) {
	    ((Painter*) entry->tag)->Clip(c, l, b, r, t);
	}
    }
}

void GraphicToPainter::NoClip() {
    Entry* entry;

    if (canvas != nil) {
	canvas = nil;

	for (int i = 0; i < count; ++i) {
	    for (entry = entries[i]; entry != nil; entry = entry->next) {
		((Painter*) entry->tag)->NoClip();
	    }
	}
    }
}
