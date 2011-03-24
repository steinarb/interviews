/*
 * Copyright (c) 1991 Stanford University
 * Copyright (c) 1991 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and 
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Stanford and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Stanford and Silicon Graphics.
 * 
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 *
 * IN NO EVENT SHALL STANFORD OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
 * OF THIS SOFTWARE.
 */

/*
 * PointerHandler - general input handler for pointer events
 */

#include <InterViews/event.h>
#include <InterViews/ptrhandler.h>

PointerHandler::PointerHandler() {
    pressed_ = false;
    aborted_ = false;
}

PointerHandler::~PointerHandler() { }

void PointerHandler::event(Event& e) {
    switch (e.type()) {
    case Event::down:
	if (pressed_) {
	    if (!aborted_) {
		abort(e);
	    }
	} else {
	    press(e);
	}
	break;
    case Event::motion:
	if (pressed_) {
	    if (!aborted_) {
		drag(e);
	    }
	} else {
	    sense(e);
	}
	break;
    case Event::up:
	if (pressed_) {
	    release(e);
	}
	break;
    default:
	/* ignore */
	break;
    }
}

void PointerHandler::sense(Event&) { }

void PointerHandler::press(Event& e) {
    pressed_ = true;
    aborted_ = false;
    e.grab(this);
    drag(e);
}

void PointerHandler::drag(Event&) { }

void PointerHandler::release(Event& e) {
    if (aborted_) {
	aborted_ = false;
    } else {
	commit(e);
    }
    e.ungrab(this);
    pressed_ = false;
}

void PointerHandler::commit(Event&) { }

void PointerHandler::abort(Event&) {
    aborted_ = true;
}
