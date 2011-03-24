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
 * Implementation of input event handling.
 */

#include <InterViews/interactor.h>
#include <InterViews/sensor.h>
#include <InterViews/world.h>
#include <string.h>

Sensor* allEvents;
Sensor* onoffEvents;
Sensor* updownEvents;
Sensor* noEvents;
Sensor* stdsensor;

extern int
    motionmask, keymask, entermask, leavemask, focusmask, substructmask,
    upmask, downmask, initmask;

Sensor::Sensor () {
    register int i;

    mask = initmask;
    timer = false;
    for (i = 0; i < 8; i++) {
	down[i] = 0;
	up[i] = 0;
    }
    Reference();
}

Sensor::Sensor (register Sensor* s) {
    register int i;

    mask = s->mask;
    timer = s->timer;
    sec = s->sec;
    usec = s->usec;
    for (i = 0; i < 8; i++) {
	down[i] = s->down[i];
	up[i] = s->up[i];
    }
    Reference();
}

Sensor::~Sensor () {
    /* nothing to do */
}

void InitSensors () {
    allEvents = new Sensor;
    allEvents->Catch(MotionEvent);
    allEvents->Catch(DownEvent);
    allEvents->Catch(UpEvent);
    allEvents->Catch(KeyEvent);
    allEvents->Catch(EnterEvent);
    allEvents->Catch(LeaveEvent);
    onoffEvents = new Sensor;
    onoffEvents->Catch(EnterEvent);
    onoffEvents->Catch(LeaveEvent);
    updownEvents = new Sensor;
    updownEvents->Catch(UpEvent);
    updownEvents->Catch(DownEvent);
    noEvents = new Sensor;
    stdsensor = noEvents;
}

void Sensor::Catch (EventType t) {
    register int i;

    switch (t) {
	case MotionEvent:
	    mask |= motionmask;
	    break;
	case DownEvent:
	    mask |= downmask;
	    SetMouseButtons(down);
	    break;
	case UpEvent:
	    mask |= upmask;
	    SetMouseButtons(up);
	    break;
	case KeyEvent:
	    mask |= keymask;
	    down[0] |= 0xfffffff8;
	    for (i = 1; i < 8; i++) {
		down[i] = 0xffffffff;
	    }
	    break;
	case EnterEvent:
	    mask |= entermask;
	    break;
	case LeaveEvent:
	    mask |= leavemask;
	    break;
	case FocusInEvent:
	case FocusOutEvent:
	    mask |= focusmask;
	    break;
	case ChannelEvent:
	    /* ignore */
	    break;
	case TimerEvent:
	    timer = true;
	    sec = 0;
	    usec = 0;
	    break;
    }
}

void Sensor::CatchButton (EventType t, int b) {
    switch (t) {
	case DownEvent:
	    mask |= downmask;
	    SetButton(down, b);
	    break;
	case UpEvent:
	    mask |= upmask;
	    SetButton(up, b);
	    break;
	case KeyEvent:
	    mask |= keymask;
	    SetButton(down, b);
	    break;
	default:
	    /* ignore */
	    break;
    }
}

void Sensor::CatchChannel (int ch) {
    channels |= (1 << ch);
    if (ch > maxchannel) {
	maxchannel = ch+1;
    }
}

void Sensor::CatchTimer (int s, int u) {
    timer = true;
    sec = s;
    usec = u;
}

void Sensor::Ignore (EventType t) {
    register int i;

    switch (t) {
	case MotionEvent:
	    mask &= ~motionmask;
	    break;
	case DownEvent:
	    ClearMouseButtons(down);
	    if (!MouseButtons(up)) {
		mask &= ~downmask;
	    }
	    break;
	case UpEvent:
	    ClearMouseButtons(up);
	    if (!MouseButtons(down)) {
		mask &= ~upmask;
	    }
	    break;
	case KeyEvent:
	    down[0] &= ~0xfffffff8;
	    for (i = 1; i < 8; i++) {
		down[i] = 0;
	    }
	    mask &= ~keymask;
	    break;
	case EnterEvent:
	    mask &= ~entermask;
	    break;
	case LeaveEvent:
	    mask &= ~leavemask;
	    break;
	case FocusInEvent:
	case FocusOutEvent:
	    mask &= ~focusmask;
	    break;
	case ChannelEvent:
	    channels = 0;
	    maxchannel = 0;
	    break;
	case TimerEvent:
	    timer = false;
	    break;
    }
}

void Sensor::IgnoreButton (EventType t, int b) {
    register int i;

    switch (t) {
	case DownEvent:
	    ClearButton(down, b);
	    if (!MouseButtons(down) && !MouseButtons(up)) {
		mask &= ~downmask;
	    }
	    break;
	case UpEvent:
	    ClearButton(up, b);
	    if (!MouseButtons(up) && !MouseButtons(down)) {
		mask &= ~upmask;
	    }
	    break;
	case KeyEvent:
	    ClearButton(down, b);
	    if ((down[0] & 0xfffffff8) == 0) {
		mask &= ~keymask;
		for (i = 1; i < 8; i++) {
		    if (down[i] != 0) {
			mask |= keymask;
			break;
		    }
		}
	    }
	    break;
	default:
	    /* ignore */
	    break;
    }
}

void Sensor::IgnoreChannel (int ch) {
    channels &= ~(1 << ch);
    if (channels == 0) {
	maxchannel = 0;
    } else {
	while ((channels & (1 << maxchannel)) == 0) {
	    --maxchannel;
	}
    }
}

void Sensor::CatchRemote () {
    mask |= substructmask;
}

void Sensor::IgnoreRemote () {
    mask &= ~substructmask;
}

Event& Event::operator= (register Event& e) {
    target = e.target;
    timestamp = e.timestamp;
    eventType = e.eventType;
    x = e.x;
    y = e.y;
    control = e.control;
    meta = e.meta;
    shift = e.shift;
    shiftlock = e.shiftlock;
    leftmouse = e.leftmouse;
    middlemouse = e.middlemouse;
    rightmouse = e.rightmouse;
    button = e.button;
    len = e.len;
    if (e.keystring == e.keydata) {
	keystring = keydata;
	strncpy(keydata, e.keydata, sizeof(keydata));
    } else {
	keystring = e.keystring;
    }
    channel = e.channel;
    w = e.w;
    wx = e.wx;
    wy = e.wy;
    return *this;
}

void Event::GetAbsolute (Coord& absx, Coord& absy) {
    if (w == nil) {
	register Interactor* r;

	for (r = target; r->Parent() != nil; r = r->Parent());
	w = (World*)r;
    }
    absx = w->InvMapX(wx);
    absy = w->InvMapY(wy);
}

void Event::GetAbsolute (World*& s, Coord& absx, Coord& absy) {
    GetAbsolute(absx, absy);
    s = w;
}
