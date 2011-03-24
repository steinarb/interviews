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
 * X11-dependent code
 */

#include "itable.h"
#include <InterViews/bitmap.h>
#include <InterViews/canvas.h>
#include <InterViews/cursor.h>
#include <InterViews/interactor.h>
#include <InterViews/sensor.h>
#include <InterViews/shape.h>
#include <InterViews/world.h>
#include <InterViews/worldview.h>
#include <InterViews/X11/eventrep.h>
#include <InterViews/X11/wmhints.h>
#include <InterViews/X11/worldrep.h>
#include <InterViews/X11/Xlib.h>
#include <InterViews/X11/Xutil.h>

void Interactor::Listen (Sensor* s) {
    Mask m;

    cursensor = s;
    if (canvas == nil) {
	/* can't set input interest without window */
	return;
    }
    m = (s == nil) ? 0 : s->mask;
    if (parent != nil) {
	/* Exposure on everyone but root window */
	m |= ExposureMask;
	if (parent->parent == nil) {
	    /* StructureNotify on top-level windows only */
	    m |= StructureNotifyMask;
	}
    }
    XSelectInput(_world->display(), (Window)canvas->id, m);
}

int Interactor::Fileno () {
    return ConnectionNumber(_world->display());
}

/*
 * Read a single event from the event stream.  If it is an input event,
 * is associated with an interactor, and the interactor is interested in it,
 * then return true.  In all other cases, return false.
 */

boolean Interactor::GetEvent (Event& e, boolean remove) {
    XEvent& xe = e.Rep()->event();
    Window w;
    Interactor* i;
    WorldView* wv;

    XNextEvent(_world->display(), &xe);
    switch (xe.type) {
	case MapNotify:
	    if (_world->itable()->Find(i, (void*)xe.xmap.window)) {
		i->SendActivate();
	    }
	    return false;
	case UnmapNotify:
	    if (_world->itable()->Find(i, (void*)xe.xunmap.window)) {
		i->SendDeactivate();
	    }
	    return false;
	case Expose:
	    if (_world->itable()->Find(i, (void*)xe.xexpose.window)) {
		i->SendRedraw(
		    xe.xexpose.x, xe.xexpose.y,
		    xe.xexpose.width, xe.xexpose.height, xe.xexpose.count
		);
	    }
	    return false;
	case ConfigureNotify:
	    if (_world->itable()->Find(i, (void*)xe.xconfigure.window)) {
		i->SendResize(
		    xe.xconfigure.x, xe.xconfigure.y,
		    xe.xconfigure.width, xe.xconfigure.height
		);
	    }
	    return false;
	case MotionNotify:
	    w = xe.xmotion.window;
	    break;
	case KeyPress:
	    w = xe.xkey.window;
	    break;
	case ButtonPress:
	case ButtonRelease:
	    w = xe.xbutton.window;
	    break;
	case FocusIn:
	case FocusOut:
	    w = xe.xfocus.window;
	    break;
	case EnterNotify:
	case LeaveNotify:
	    w = xe.xcrossing.window;
	    break;
	case MapRequest:
	    wv = _worldview;
	    if (wv != nil &&
		wv->canvas->id == (void*)xe.xmaprequest.parent
	    ) {
		wv->InsertRemote((void*)xe.xmaprequest.window);
	    }
	    return false;
	case ConfigureRequest:
	    wv = _worldview;
	    if (wv != nil &&
		wv->canvas->id == (void*)xe.xconfigurerequest.parent
	    ) {
		wv->ChangeRemote(
		    (void*)xe.xconfigurerequest.window,
		    xe.xconfigurerequest.x, ymax - xe.xconfigurerequest.y,
		    xe.xconfigurerequest.width, xe.xconfigurerequest.height
		);
	    }
	    return false;
	default:
	    /* ignore */
	    return false;
    }
    /* only input events should get here */
    if (!_world->itable()->Find(i, (void*)w) ||
	i->cursensor == nil || !i->cursensor->Interesting(e)
    ) {
	return false;
    }
    e.target = i;

    /*
     * Can't do the QueryPointer before checking to make sure the window
     * is valid because a motion event might have been sent just before
     * we destroyed the target window.
     */
    if (xe.type == MotionNotify) {
	Window dummy;
	int x, y, wx, wy;
	unsigned int state;

	XQueryPointer(
            _world->display(), w, &dummy, &dummy, &x, &y, &wx, &wy, &state
        );
	e.x = wx;
	e.y = wy;
    }
    e.y = i->ymax - e.y;
    if (!remove) {
	XPutBackEvent(_world->display(), &xe);
    }
    return true;
}

/*
 * Check to see if any input events of interest are pending.
 * This routine will return true even if the event is for another interactor.
 */

boolean Interactor::Check () {
    Event e;

    while (XPending(_world->display())) {
	if (GetEvent(e, false)) {
	    return true;
	}
    }
    return false;
}

int Interactor::CheckQueue () {
    return QLength(_world->display());
}

void Interactor::SendRedraw (Coord x, Coord y, int w, int h, int count) {
    if (count == 0) {
	Coord top = ymax - y;
	Redraw(x, top - h + 1, x + w - 1, top);
    } else {
	register int i;
	XEvent xe;
	Coord buf[40];
	Coord* left, * bottom, * right, * top;

	i = count + 1;
	if (i <= 10) {
	    left = buf;
	} else {
	    left = new Coord[4*i];
	}
	bottom = &left[i];
	right = &bottom[i];
	top = &right[i];

	left[0] = x;
	top[0] = ymax - y;
	right[0] = x + w - 1;
	bottom[0] = top[0] - h + 1;
	for (i = 1; i <= count; i++) {
	    /*
	     * This should be XNextEvent(_world->display(), xe), but that
	     * didn't work.  I think that implies an X server bug,
	     * but I have to live with what I've got for now.
	     */
	    XWindowEvent(
                _world->display(), (Window)canvas->id, ExposureMask, &xe
            );
	    left[i] = xe.xexpose.x;
	    top[i] = ymax - xe.xexpose.y;
	    right[i] = left[i] + xe.xexpose.width - 1;
	    bottom[i] = top[i] - xe.xexpose.height + 1;
	}
	RedrawList(count+1, left, bottom, right, top);
	if (left != buf) {
	    delete left;
	}
    }
}

void Interactor::SendResize (Coord x, Coord y, int w, int h) {
    left = x;
    bottom = parent->ymax - y - h + 1;
    if (canvas->width != w || canvas->height != h) {
        canvas->width = w;
        canvas->height = h;
        xmax = w - 1;
        ymax = h - 1;
        Resize();
    }
}

void Interactor::SendActivate () {
    canvas->status = CanvasMapped;
    Activate();
}

void Interactor::SendDeactivate () {
    canvas->status = CanvasUnmapped;
    Deactivate();
}

void Interactor::Poll (Event& e) {
    Window root, child;
    int x, y, root_x, root_y;
    unsigned int state;
    register unsigned int s;
    Interactor* i;

    XQueryPointer(
	_world->display(), (Window)canvas->id, &root, &child,
	&root_x, &root_y, &x, &y, &state
    );
    e.x = x;
    e.y = ymax - y;
    if (_world->itable()->Find(i, (void*)root)) {
	e.w = (World*)i;
    } else {
	e.w = nil;
    }
    e.wx = root_x;
    e.wy = root_y;
    s = state;
    e.control = (s&ControlMask) != 0;
    e.meta = (s&Mod1Mask) != 0;
    e.shift = (s&ShiftMask) != 0;
    e.shiftlock = (s&LockMask) != 0;
    e.leftmouse = (s&Button1Mask) != 0;
    e.middlemouse = (s&Button2Mask) != 0;
    e.rightmouse = (s&Button3Mask) != 0;
}

void Interactor::Flush () {
    XFlush(_world->display());
}

void Interactor::Sync () {
    XSync(_world->display(), 0);
}

void Interactor::GetRelative (Coord& x, Coord& y, Interactor* rel) {
    register Interactor* t, * r;
    Coord tx, ty, rx, ry;

    if (parent == nil) {
	if (rel == nil || rel->parent == nil) {
	    /* world relative to world -- nop */
	    return;
	}
	/* world relative to interactor is relative to interactor's l, b */
	rx = 0; ry = 0;
	rel->GetRelative(rx, ry);
	x = x - rx;
	y = y - ry;
	return;
    }
    tx = x; ty = y;
    t = this;
    for (t = this; t->parent->parent != nil; t = t->parent) {
	tx += t->left;
	ty += t->bottom;
    }
    if (rel == nil || rel->parent == nil) {
	r = nil;
    } else {
	rx = 0; ry = 0;
	for (r = rel; r->parent->parent != nil; r = r->parent) {
	    rx += r->left;
	    ry += r->bottom;
	}
    }
    if (r == t) {
	/* this and rel are within same top-level interactor */
	x = tx - rx;
	y = ty - ry;
    } else {
	Interactor* w;
	Window child;

	w = (rel == nil) ? t->parent : rel;
	XTranslateCoordinates(
	    _world->display(), (Window)canvas->id, (Window)w->canvas->id,
	    x, ymax - y, &rx, &ry, &child
	);
	x = rx;
	y = w->ymax - ry;
    }
}

void Interactor::DoSetCursor (Cursor* c) {
    if (c == nil) {
	XUndefineCursor(_world->display(), (Window)canvas->id);
    } else {
	XDefineCursor(_world->display(), (Window)canvas->id, (XCursor)c->Id());
    }
    Flush();
}

void Interactor::DoSetName (const char* s) {
    XStoreName(_world->display(), (Window)canvas->id, s);
}

void Interactor::DoSetGroupLeader (Interactor* leader) {
    WindowMgrHints wm(_world->display(), (Window)canvas->id);
    Canvas* leadercanvas = leader ? leader->canvas : nil;
    wm.WindowGroup(leader, leadercanvas);
    wm.Set(_world->display(), (Window)canvas->id);
}

void Interactor::DoSetTransientFor (Interactor* owner) {
    if (owner != nil && owner->canvas != nil) {
	Window w = (Window)canvas->id;
	XSetTransientForHint(_world->display(), w, (Window)owner->canvas->id);
    }
    /* else clear the hint, but how? */
}

void Interactor::DoSetIconName (const char* name) {
    XSetIconName(_world->display(), (Window)canvas->id, name);
}

void Interactor::DoSetIconBitmap (Bitmap* bitmap) {
    WindowMgrHints wm(_world->display(), (Window)canvas->id);
    wm.IconPixmap(bitmap);
    wm.Set(_world->display(), (Window)canvas->id);
}

void Interactor::DoSetIconMask (Bitmap* mask) {
    WindowMgrHints wm(_world->display(), (Window)canvas->id);
    wm.IconMask(mask);
    wm.Set(_world->display(), (Window)canvas->id);
}

void Interactor::DoSetIconInteractor (Interactor* icon) {
    WindowMgrHints wm(_world->display(), (Window)canvas->id);
    Canvas* dummycanvas = nil;
    Canvas*& iconcanvas = icon ? icon->canvas : dummycanvas;
    wm.IconWindow(this, icon, iconcanvas);
    wm.Set(_world->display(), (Window)canvas->id);
}

void Interactor::DoSetIconGeometry (const char* g) {
    WindowMgrHints wm(_world->display(), (Window)canvas->id);
    wm.IconPosition(this, g);
    wm.Set(_world->display(), (Window)canvas->id);
}

void Interactor::Iconify () {
    static Atom XA_WM_CHANGE_STATE = None; /* remove when defined in Xatom.h */
    Bool propagate = False;
    Mask eventmask = (SubstructureRedirectMask|SubstructureNotifyMask);
    XEvent xe;

    if (XA_WM_CHANGE_STATE == None) {
	XA_WM_CHANGE_STATE = XInternAtom(
            _world->display(), "WM_CHANGE_STATE", False
        );
    }

    xe.type = ClientMessage;
    xe.xclient.type = ClientMessage;
    xe.xclient.display = _world->display();
    xe.xclient.window = (Window)canvas->id;
    xe.xclient.message_type = XA_WM_CHANGE_STATE;
    xe.xclient.format = 32;
    xe.xclient.data.l[0] = IconicState;

    XSendEvent(_world->display(), _world->root(), propagate, eventmask, &xe);
}

void Interactor::DeIconify () {
    if (canvas != nil) {
	XMapWindow(_world->display(), (Window)canvas->id);
	canvas->status = CanvasMapped;
    }
}
