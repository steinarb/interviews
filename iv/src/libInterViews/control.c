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
 * Implementation of control classes
 */

#include <InterViews/control.h>
#include <InterViews/message.h>
#include <InterViews/sensor.h>

/** class Control **/

Control::Control(Interactor* i) { Init(nil, i); }
Control::Control(const char* name, Interactor* i) { Init(name, i); }

void Control::Init(const char* name, Interactor* i) {
    SetClassName("Control");
    if (name != nil) {
	SetInstance(name);
    }
    state = new ControlState;
    state->Attach(this);
    input = new Sensor;
    input->Catch(EnterEvent);
    input->Catch(LeaveEvent);
    input->Catch(DownEvent);
    input->Catch(UpEvent);
    if (i != nil) {
	Insert(i);
    }
}

Control::~Control () {
    state->Detach(this);
}

void Control::Handle(Event& e) {
    switch (e.eventType) {
    case DownEvent:
	Down();
	break;
    case UpEvent:
	Up();
	break;
    case EnterEvent:
	Enter();
	break;
    case LeaveEvent:
	Leave();
	break;
    }
}

/*
 * Hitting a control is equivalent to making the state active and
 * then selecting it.
 */

void Control::Down() {
    if (!state->Active()) {
	state->Activate();
	state->NotifySelection(this);
    }
}

void Control::Enter() {
    if (state->Active()) {
	state->NotifySelection(this);
    }
}

void Control::Leave() {
    if (state->Active()) {
	state->NotifySelection(nil);
    }
}

void Control::Select() {
    Highlight(true);
    Open();
    Grab();
}

void Control::Unselect() {
    Close();
    Highlight(false);
}

void Control::Grab() {
    Event e;
    do {
	Read(e);
	e.target->Handle(e);
	if (e.target == this && e.eventType == LeaveEvent) {
	    Skip();
	    break;
	}
    } while (state->Active());
}

/*
 * Skip all input events until we see something for another open control.
 */

void Control::Skip() {
    Event e;
    for (;;) {
	Read(e);
	if (e.eventType == EnterEvent && IsGrabbing(e.target)) {
	    UnRead(e);
	    break;
	} else if (e.eventType == UpEvent) {
	    Up();
	    break;
	}
    }
}

/*
 * Check whether an interactor (usually an event target) is a grabbing
 * control.  Trivially, the current control is grabbing.  Any other controls
 * attached to the control's state are also grabbing.  Similarly, any controls
 * attached to other control's state up the stack from the current one
 * are also considered to be grabbing.
 */

boolean Control::IsGrabbing(Interactor* i) {
    if (i == this) {
	return true;
    }
    for (ControlState* c = state; c != nil; c = c->Next()) {
	if (c->IsView(i)) {
	    return true;
	}
    }
    for (c = state->Prev(); c != nil; c = c->Prev()) {
	if (c->IsView(i)) {
	    return true;
	}
    }
    return false;
}

/*
 * On an up event, deactivate all the control states and then
 * call Do the current selection (if any).
 */

void Control::Up() {
    if (state->Active()) {
	Control* target = state->Selection();
	state->Action(target);
	for (ControlState* c = state; c != nil; c = c->Prev()) {
	    c->Deactivate();
	}
	if (target != nil) {
	    target->Do();
	}
    }
}

void Control::Open() { }
void Control::Close() { }
void Control::Do() { }

void Control::SetState(ControlState* s) {
    state->Detach(this);
    state = s;
    s->Attach(this);
}

/** class ControlState **/

ControlState::ControlState(unsigned s) {
    status = s;
    selection = nil;
    action = nil;
    next = nil;
    prev = nil;
}

ControlState::~ControlState() { }

void ControlState::NotifySelection(Control* c) {
    if (selection != c) {
	if (selection != nil) {
	    selection->Unselect();
	}
	selection = c;
	if (selection != nil) {
	    selection->Select();
	}
    }
}

void ControlState::Push(ControlState* s) {
    next = s;
    s->prev = this;
}

void ControlState::Pop() {
    if (prev != nil) {
	prev->next = next;
	prev = nil;
    }
    Deactivate();
}

void ControlState::Deactivate() {
    Set(ControlActive, false);
    if (selection != nil) {
	selection->Unselect();
	selection = nil;
    }
}
