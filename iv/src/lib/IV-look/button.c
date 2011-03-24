/*
 * Copyright (c) 1987, 1988, 1989, 1990, 1991 Stanford University
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
 * Button - clickable Action
 */

#include <IV-look/button.h>
#include <IV-look/telltale.h>
#include <InterViews/action.h>
#include <InterViews/event.h>
#include <InterViews/listener.h>

Button::Button(Action* a, Telltale* t) : MonoGlyph(nil), PointerHandler() {
    action_ = a;
    Resource::ref(action_);
    telltale_ = t;
    Resource::ref(telltale_);
    listener_ = new Listener(telltale_, this);
    listener_->button(true, Event::any);
    listener_->motion(true);
    body(listener_);
}

Button::~Button() {
    Resource::unref(action_);
    action_ = nil;
    Resource::unref(telltale_);
    telltale_ = nil;
}

void Button::drag(Event& e) {
    if (telltale_ != nil) {
	telltale_->highlight(listener_->picks(e.pointer_x(), e.pointer_y()));
    }
}

void Button::release(Event& e) {
    if (telltale_ != nil) {
	telltale_->highlight(false);
    }
    PointerHandler::release(e);
}

void Button::commit(Event& e) {
    if (action_ != nil && listener_->picks(e.pointer_x(), e.pointer_y())) {
	action_->execute();
    }
}
