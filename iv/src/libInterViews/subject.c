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
 * Implementation of subject base class.
 */

#include <InterViews/interactor.h>
#include <InterViews/subject.h>

Subject::Subject() {
    views = nil;
}

Subject::~Subject() {
    register ViewList* v, * next;

    for (v = views; v != nil; v = next) {
	next = v->next;
	delete v;
    }
}

void Subject::Attach(Interactor* i) {
    ViewList* v = new ViewList;
    v->element = i;
    v->next = views;
    views = v;
    Reference();
}

void Subject::Detach(Interactor* i) {
    register ViewList* v, * prev;

    prev = nil;
    for (v = views; v != nil; v = v->next) {
	if (v->element == i) {
	    if (prev == nil) {
		views = v->next;
	    } else {
		prev->next = v->next;
	    }
	    delete v;
	    Unreference();
	    break;
	}
	prev = v;
    }
}

void Subject::all(InteractorItr& i) {
    i.cur = views;
}

void Subject::Notify() {
    for (InteractorItr i(this); i.more(); i.next()) {
	i.view()->Update();
    }
}

boolean Subject::IsView(Interactor* v) {
    for (InteractorItr i(this); i.more(); i.next()) {
	if (i.view() == v) {
	    return true;
	}
    }
    return false;
}
