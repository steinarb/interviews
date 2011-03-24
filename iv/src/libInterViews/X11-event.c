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
 * X11-dependent event code
 */

#include "itable.h"
#include <InterViews/event.h>
#include <InterViews/X11/eventrep.h>
#include <InterViews/X11/worldrep.h>
#include <InterViews/X11/Xlib.h>
#include <InterViews/X11/Xutil.h>
#include <string.h>

Event::Event() {
    rep = new EventRep;
}

Event::~Event() {
    delete rep;
}

void Event::GetMotionInfo() {
    register XMotionEvent& m = rep->event().xmotion;
    eventType = MotionEvent;
    timestamp = m.time;
    x = m.x;
    y = m.y;
    wx = m.x_root;
    wy = m.y_root;
    GetKeyState(m.state);
    FindWorld(m.root);
}

void Event::GetButtonInfo(EventType t) {
    register XButtonEvent& b = rep->event().xbutton;
    eventType = t;
    timestamp = b.time;
    x = b.x;
    y = b.y;
    wx = b.x_root;
    wy = b.y_root;
    button = b.button - 1;
    len = 0;
    GetKeyState(b.state | (Button1Mask << button));
    FindWorld(b.root);
}

void Event::GetKeyInfo() {
    register XKeyEvent& k = rep->event().xkey;
    char buf[4096];

    eventType = KeyEvent;
    timestamp = k.time;
    x = k.x;
    y = k.y;
    wx = k.x_root;
    wy = k.y_root;
    button = k.keycode;
    len = XLookupString(&k, buf, sizeof(buf), nil, nil);
    if (len != 0) {
	if (len < sizeof(keydata)) {
	    keystring = keydata;
	} else {
	    keystring = new char[len+1];
	}
	strncpy(keystring, buf, len);
	keystring[len] = '\0';
    } else {
	keystring = keydata;
	keydata[0] = '\0';
    }
    GetKeyState(k.state);
    FindWorld(k.root);
}

void Event::GetKeyState(register unsigned state) {
    shift = (state & ShiftMask) != 0;
    control = (state & ControlMask) != 0;
    meta = (state & Mod1Mask) != 0;
    shiftlock = (state & LockMask) != 0;
    leftmouse = (state & Button1Mask) != 0;
    middlemouse = (state & Button2Mask) != 0;
    rightmouse = (state & Button3Mask) != 0;
}

boolean Event::GetCrossingInfo(EventType t) {
    register XCrossingEvent& c = rep->event().xcrossing;
    if (c.detail != NotifyInferior) {
	eventType = t;
	timestamp = c.time;
	x = c.x;
	y = c.y;
	wx = c.x_root;
	wy = c.y_root;
	GetKeyState(c.state);
	FindWorld(c.root);
	return true;
    }
    return false;
}

void Event::FindWorld(unsigned long root) {
    Interactor* i;

    if (_world->itable()->Find(i, (void*)root)) {
        w = (World*)i;
    } else {
	w = nil;
    }
}
