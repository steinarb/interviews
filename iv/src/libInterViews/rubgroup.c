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
 * RubberGroup implementation.
 */

#include <InterViews/rubgroup.h>

class RubberList {
private:
    RubberList* next;
    RubberList* prev;
public:
    Rubberband* rub;

    RubberList(Rubberband* =nil);
    ~RubberList();

    void Append(Rubberband*);
    void Prepend(Rubberband*);
    void Delete(RubberList*);
    RubberList* Find(Rubberband*);
    RubberList* Next() { return next; }
    RubberList* Prev() { return prev; }
    RubberList* End() { return this; }
    boolean IsEmpty() { return next == this; }
};

RubberList::RubberList (Rubberband* r) {
    rub = r;
    next = this;
    prev = this;
}

RubberList::~RubberList () {
    RubberList* doomed;

    while (next != this) {
	doomed = next;
	next = doomed->next;
	doomed->next = doomed;
	delete doomed;
    }
    if (rub != nil) {
	delete rub;
    }
}

void RubberList::Append (Rubberband* b) {
    RubberList* cur = new RubberList(b);

    cur->next = this;
    cur->prev = prev;
    prev->next = cur;
    prev = cur;
}

void RubberList::Prepend (Rubberband* b) {
    RubberList* cur = new RubberList(b);

    cur->next = next;
    cur->prev = this;
    next->prev = cur;
    next = cur;
}

void RubberList::Delete (RubberList* elem) {
    elem->prev->next = elem->next;
    elem->next->prev = elem->prev;
    elem->next = elem;
    delete elem;
}

RubberList* RubberList::Find (Rubberband* target) {
    RubberList* r;
    
    for (r = Next(); r != this; r = r->Next()) {
	if (r->rub == target) {
	    return r;
	}
    }
    return nil;
}

/*****************************************************************************/

RubberGroup::RubberGroup (Painter* p, Canvas* c) : (p, c, 0, 0) {
    list = cur = new RubberList;
}

void RubberGroup::Draw () {
    for (RubberList* r = list->Next(); r != list->End(); r = r->Next()) {
	r->rub->Draw();
    }
}

void RubberGroup::Erase () {
    for (RubberList* r = list->Next(); r != list->End(); r = r->Next()) {
	r->rub->Erase();
    }
}

void RubberGroup::Track (Coord x, Coord y) {
    for (RubberList* r = list->Next(); r != list->End(); r = r->Next()) {
	r->rub->Track(x, y);
    }
}

RubberGroup::~RubberGroup () { delete list; }

void RubberGroup::SetPainter (Painter* p) {
    Rubberband::SetPainter(p);

    for (RubberList* r = list->Next(); r != list->End(); r = r->Next()) {
	r->rub->SetPainter(p);
    }
}

void RubberGroup::SetCanvas (Canvas* c) {
    Rubberband::SetCanvas(c);

    for (RubberList* r = list->Next(); r != list->End(); r = r->Next()) {
	r->rub->SetCanvas(c);
    }
}

void RubberGroup::Append (
    Rubberband* r1, Rubberband* r2, Rubberband* r3, Rubberband* r4
) {
    list->Append(r1);
    if (r2 != nil) {
	list->Append(r2);

        if (r3 != nil) {
            list->Append(r3);

            if (r4 != nil) {
                list->Append(r4);
            }
        }
    }
}

void RubberGroup::Remove (Rubberband* target) {
    RubberList* r = list->Find(target);
    
    if (r != nil) {
	cur = cur->Next();
	list->Delete(r);
    }
}

void RubberGroup::RemoveCur () {
    RubberList* doomed = cur;
    
    if (!AtEnd()) {
	cur = cur->Next();
	list->Delete(doomed);
    }
}

void RubberGroup::SetCurrent (Rubberband* target) {
    RubberList* r = list->Find(target);
    
    if (r != nil) {
	cur = r;
    }
}

Rubberband* RubberGroup::GetCurrent () {
    return cur->rub;
}

Rubberband* RubberGroup::First () {
    cur = list->Next(); 
    return cur->rub;
}
Rubberband* RubberGroup::Last () {
    cur = list->Prev(); 
    return cur->rub;
}

Rubberband* RubberGroup::Next () {
    cur = list->Next(); 
    return cur->rub;
}

Rubberband* RubberGroup::Prev () {
    cur = list->Prev(); 
    return cur->rub;
}

boolean RubberGroup::AtEnd () { return cur == list; }
boolean RubberGroup::IsEmpty () { return list->IsEmpty(); }
