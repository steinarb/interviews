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
 * Dialog class implementation.
 */

#include <InterViews/button.h>
#include <InterViews/dialog.h>
#include <InterViews/event.h>
#include <InterViews/world.h>

Dialog::Dialog (ButtonState* b, Interactor* i, Alignment a) {
    Init(b, i, a);
}

Dialog::Dialog (const char* name, ButtonState* b, Interactor* i, Alignment a) {
    SetInstance(name);
    Init(b, i, a);
}

void Dialog::Init (ButtonState* b, Interactor* i, Alignment a) {
    SetClassName("Dialog");
    state = b;
    align = a;
    Insert(i);
}

boolean Dialog::Popup (Event& e, boolean placed) {
    World* w;
    Coord wx, wy;
    boolean accept;

    e.GetAbsolute(w, wx, wy);
    if (placed) {
	w->InsertTransient(this, e.target, wx, wy, align);
    } else {
	w->InsertTransient(this, e.target);
    }
    accept = Accept();
    w->Remove(this);
    return accept;
}

boolean Dialog::Accept () {
    Event e;
    int v;

    state->SetValue(0);
    v = 0;
    do {
	Read(e);
	e.target->Handle(e);
	state->GetValue(v);
    } while (v == 0 && e.target != nil);
    return v == 1 || e.target == nil;
}

int Dialog::Status () {
    int v;

    state->GetValue(v);
    return v;
}
