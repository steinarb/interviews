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
 * X11-dependent worldview code
 */

#include <InterViews/canvas.h>
#include <InterViews/cursor.h>
#include <InterViews/painter.h>
#include <InterViews/shape.h>
#include <InterViews/world.h>
#include <InterViews/worldview.h>
#include <InterViews/X11/worldrep.h>
#include <InterViews/X11/Xlib.h>
#include <InterViews/X11/Xutil.h>
#include <X11/Xatom.h>
#include <os/timing.h>

void WorldView::Init (World* world) {
    WorldRep* rep = world->Rep();
    Window w = RootWindow(rep->display(), rep->screen());
    canvas = new Canvas((void*)w);
    canvas->width = DisplayWidth(rep->display(), rep->screen());
    canvas->height = DisplayHeight(rep->display(), rep->screen());
    canvas->status = CanvasMapped;
    xmax = canvas->width - 1;
    ymax = canvas->height - 1;
    output->SetOverwrite(true);
}

static const Mask bmask =
    ButtonPressMask|ButtonReleaseMask|OwnerGrabButtonMask|
    PointerMotionMask|PointerMotionHintMask;

void WorldView::GrabMouse (Cursor* c) {
    while (
	XGrabPointer(
	    world->Rep()->display(), (Window)canvas->id, True,
	    (unsigned int)bmask, GrabModeAsync, GrabModeAsync, None,
	    (XCursor)c->Id(), CurrentTime
	) != GrabSuccess
    ) {
	sleep(1);
    }
}

void WorldView::UngrabMouse () {
    XUngrabPointer(world->Rep()->display(), CurrentTime);
}

boolean WorldView::GrabButton (unsigned b, unsigned m, Cursor* c) {
    XGrabButton(
	world->Rep()->display(), b, m, (Window)canvas->id, True,
	(unsigned int)bmask, GrabModeAsync, GrabModeAsync, None,
	(XCursor)c->Id()
    );
    return true;
}

void WorldView::UngrabButton (unsigned b, unsigned m) {
    XUngrabButton(world->Rep()->display(), b, m, (Window)canvas->id);
}

void WorldView::Lock () {
/*
 * Bad idea to grab the server
 */
}

void WorldView::Unlock () {
/*
 * See Lock()
 */
}

void WorldView::ClearInput () {
    XSync(world->Rep()->display(), 1);
}

void WorldView::MoveMouse (Coord x, Coord y) {
    XWarpPointer(
	world->Rep()->display(), (Window)canvas->id, (Window)canvas->id,
	0, 0, xmax, ymax, x, ymax - y
    );
}

void WorldView::Map (RemoteInteractor i) {
    XMapWindow(world->Rep()->display(), (Window)i);
}

void WorldView::MapRaised (RemoteInteractor i) {
    XMapRaised(world->Rep()->display(), (Window)i);
}

void WorldView::Unmap (RemoteInteractor i) {
    XUnmapWindow(world->Rep()->display(), (Window)i);
}

RemoteInteractor WorldView::Find (Coord x, Coord y) {
    Window w;
    Coord rx, ry;

    XTranslateCoordinates(
	world->Rep()->display(), (Window)canvas->id, (Window)canvas->id,
	x, ymax - y, &rx, &ry, &w
    );
    return (void*)w;
}

void WorldView::Move (RemoteInteractor i, Coord left, Coord top) {
    XMoveWindow(world->Rep()->display(), (Window)i, left, ymax - top);
}

void WorldView::Change (
    RemoteInteractor i, Coord left, Coord top, unsigned int w, unsigned int h
) {
    XMoveResizeWindow(
	world->Rep()->display(), (Window)i, left, ymax - top, w, h
    );
}

void WorldView::Raise (RemoteInteractor i) {
    XRaiseWindow(world->Rep()->display(), (Window)i);
}

void WorldView::Lower (RemoteInteractor i) {
    XLowerWindow(world->Rep()->display(), (Window)i);
}

void WorldView::Focus (RemoteInteractor i) {
    if (i != curfocus) {
	curfocus = i;
	XSetInputFocus(
	    world->Rep()->display(), i == nil ? PointerRoot : (Window)i,
	    RevertToPointerRoot, CurrentTime
	);
    }
}

void WorldView::GetList (RemoteInteractor*& ilist, unsigned int& n) {
    Window parent;

    XQueryTree(
	world->Rep()->display(), (Window)canvas->id, &parent, &parent,
	(Window**)&ilist, &n
    );
}

void WorldView::GetInfo (
    RemoteInteractor i, Coord& x1, Coord& y1, Coord& x2, Coord& y2
) {
    Window root;
    int x, y;
    unsigned int w, h, bw, d;

    XGetGeometry(
	world->Rep()->display(), (Window)i, &root, &x, &y, &w, &h, &bw, &d
    );
    x1 = x;
    y2 = ymax - y;
    x2 = x + w + 2*bw - 1;
    y1 = y2 - h - 2*bw + 1;
}

boolean WorldView::GetHints (
    RemoteInteractor i, Coord& x, Coord& y, Shape& s
) {
    XSizeHints sizehints;

    sizehints.flags = 0;
    XGetSizeHints(
	world->Rep()->display(), (Window)i, &sizehints, XA_WM_NORMAL_HINTS
    );
    if ((sizehints.flags & USSize) != 0) {
	s.width = sizehints.width;
	s.height = sizehints.height;
	s.hstretch = sizehints.max_width - sizehints.width;
	s.hshrink = sizehints.width - sizehints.min_width;
	s.vstretch = sizehints.max_height - sizehints.height;
	s.vshrink = sizehints.height - sizehints.min_height;
	s.hunits = sizehints.width_inc;
	s.vunits = sizehints.height_inc;
    } else {
	s.width = 0;
	s.height = 0;
    }
    if ((sizehints.flags & USPosition) != 0) {
	x = sizehints.x;
	y = ymax - sizehints.y;
	return true;
    }
    return false;
}

void WorldView::SetHints (RemoteInteractor i, Coord x, Coord y, Shape& s) {
    XSizeHints sizehints;

    sizehints.flags = (USPosition | USSize);
    sizehints.x = x;
    sizehints.y = ymax - y;
    sizehints.width = s.width;
    sizehints.height = s.height;
    XSetSizeHints(
	world->Rep()->display(), (Window)i, &sizehints, XA_WM_NORMAL_HINTS
    );
}

RemoteInteractor WorldView::GetIcon (RemoteInteractor i) {
    XWMHints* h;
    RemoteInteractor r;

    h = XGetWMHints(world->Rep()->display(), (Window)i);
    if (h == nil || (h->flags&IconWindowHint) == 0) {
	r = nil;
    } else {
	r = (void*)h->icon_window;
    }
    delete h;
    return r;
}

/* obsolete - window mgrs should set WM_STATE on clients' toplevel windows */

void WorldView::AssignIcon (RemoteInteractor i, RemoteInteractor icon) {
    XWMHints h;

    h.flags = IconWindowHint;
    h.icon_window = (Window)i;
    XSetWMHints(world->Rep()->display(), (Window)icon, &h);
    h.icon_window = (Window)icon;
    XSetWMHints(world->Rep()->display(), (Window)i, &h);
}

void WorldView::UnassignIcon (RemoteInteractor i) {
    XWMHints h;

    h.flags = IconWindowHint;
    h.icon_window = None;
    XSetWMHints(world->Rep()->display(), (Window)i, &h);
}

RemoteInteractor WorldView::TransientOwner (RemoteInteractor i) {
    Window w;

    return
	XGetTransientForHint(world->Rep()->display(), (Window)i, &w) ?
	    (void*)w : nil;
}

char* WorldView::GetName (RemoteInteractor i) {
    char* name;

    XFetchName(world->Rep()->display(), (Window)i, &name);
    return name;
}
