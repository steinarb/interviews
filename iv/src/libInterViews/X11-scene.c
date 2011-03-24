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
 * X11-dependent scene code
 */

#include "itable.h"
#include <InterViews/bitmap.h>
#include <InterViews/canvas.h>
#include <InterViews/cursor.h>
#include <InterViews/interactor.h>
#include <InterViews/scene.h>
#include <InterViews/shape.h>
#include <InterViews/world.h>
#include <InterViews/X11/wmhints.h>
#include <InterViews/X11/worldrep.h>
#include <InterViews/X11/Xlib.h>
#include <InterViews/X11/Xutil.h>
#include <X11/Xatom.h>
#include <string.h>

/*
 * Adjust window dimensions to avoid 0 width or height error.
 */

static void DefaultShape (int w, int h, int& nw, int& nh) {
    nw = (w == 0) ? round(2*inch) : w;
    nh = (h == 0) ? round(2*inch) : h;
}

/*
 * Place an interactor according to user preferences.
 */

void Scene::UserPlace (Interactor* i, int w, int h) {
    int width, height;
    DefaultShape(w, h, width, height);
    MakeWindow(i, 0, 0, width, height);
    SetWindowProperties(i, 0, 0, width, height, false);
    if (i->GetInteractorType() == IconInteractor) {
	Assign(i, 0, 0, width, height);
    } else {
	DoMap(i, width, height);
    }
}

/*
 * Place an interactor at a particular position.
 */

void Scene::Place (
    Interactor* i, Coord l, Coord b, Coord r, Coord t, boolean map
) {
    Coord newtop = ymax - t;
    int width, height;
    DefaultShape(r - l + 1, t - b + 1, width, height);
    MakeWindow(i, l, newtop, width, height);
    SetWindowProperties(i, l, newtop, width, height, true);
    Assign(i, l, b, width, height);
    if (map && i->GetInteractorType() != IconInteractor) {
	Map(i);
    }
}

/*
 * Pick the right set of window attributes for the interactor's window.
 * For ICCCM compatibility, only popup windows use override_redirect. As
 * a convenience, popup and transient windows automatically use save_under.
 */

static unsigned int PickWindowAttributes (
    Interactor* i, XSetWindowAttributes& a
) {
    register Mask m = 0;

    if (i->GetInteractorType() != InteriorInteractor) {
	m |= CWDontPropagate;
	a.do_not_propagate_mask = (
	    KeyPressMask | KeyReleaseMask |
	    ButtonPressMask | ButtonReleaseMask | PointerMotionMask
	);
    }

    Cursor* c = i->GetCursor();
    if (c != nil) {
	m |= CWCursor;
	a.cursor = (XCursor)c->Id();
    } else if (i->GetInteractorType() == InteriorInteractor) {
	m |= CWCursor;
	a.cursor = None;
    } else {
	m |= CWCursor;
	a.cursor = (XCursor)defaultCursor->Id();
    }

    CanvasType ct = i->GetCanvasType();
    switch (ct) {
	case CanvasSaveUnder:
	    m |= CWSaveUnder;
	    a.save_under = True;
	    break;
	case CanvasSaveContents:
	    m |= CWBackingStore;
	    a.backing_store = WhenMapped;
	    break;
	case CanvasSaveBoth:
	    m |= CWSaveUnder;
	    a.save_under = True;
	    m |= CWBackingStore;
	    a.backing_store = WhenMapped;
	    break;
    }

    switch (i->GetInteractorType()) {
	case InteriorInteractor:
	    if (ct != CanvasShapeOnly && ct != CanvasInputOnly) {
		m |= CWBackPixmap;
		a.background_pixmap = ParentRelative;
		m |= CWWinGravity;
		a.win_gravity = UnmapGravity;
	    }
	    break;
	case PopupInteractor:
	    m |= CWOverrideRedirect;
	    a.override_redirect = True;
	    if (ct != CanvasShapeOnly && ct != CanvasInputOnly) {
		m |= CWSaveUnder;
		a.save_under = True;
	    }
	    break;
	case TransientInteractor:
	    if (ct != CanvasShapeOnly && ct != CanvasInputOnly) {
		m |= CWSaveUnder;
		a.save_under = True;
	    }
	    break;
    }    

    return m;
}

/*
 * Pick the right class for the Interactor's window.
 */

static int PickWindowClass (Interactor* i) {
    int wclass = InputOutput;
    if (i->GetCanvasType() == CanvasInputOnly) {
	wclass = InputOnly;
    }
    return wclass;
}

/*
 * Pick the right set of changes to the geometry of the interactor's window.
 */

static unsigned int PickWindowChanges (
    Canvas* canvas, Coord x, Coord y, int width, int height, XWindowChanges& c
) {
    register unsigned int m = 0;

    m |= CWX;
    c.x = x;
    m |= CWY;
    c.y = y;

    if (canvas->Width() != width) {
	m |= CWWidth;
	c.width = width;
    }
    if (canvas->Height() != height) {
	m |= CWHeight;
	c.height = height;
    }

    return m;
}

/*
 * Create a window for an interactor.  If a window already exists,
 * update its geometry, attributes, and event mask.
 */

void Scene::MakeWindow (
    Interactor* i, Coord x, Coord y, int width, int height
) {
    XSetWindowAttributes a;
    InteractorType t = i->GetInteractorType();
    if (parent == nil) {
	if (t == InteriorInteractor) {
	    /* backward compatibility for world->Insert(interactor) */
	    i->SetInteractorType(ToplevelInteractor);
	}
    } else if (t != InteriorInteractor) {
	/* parent != nil, should be interior now */
	i->SetInteractorType(InteriorInteractor);
    }
    if (i->canvas == nil) {
	unsigned int amask = PickWindowAttributes(i, a);
	Window w = XCreateWindow(
	    _world->display(), (Window)canvas->id, x, y, width, height, 0, 
	    /* CopyFromParent */ 0, PickWindowClass(i),
	    /* cast below is workaround for cfront 2.0 bug */
	    (Visual*)CopyFromParent, amask, &a
	);
	i->canvas = new Canvas((void*)w);
	_world->itable()->Insert((void*)w, i);
    } else {
	XWindowChanges c;
	unsigned int cmask = PickWindowChanges(
	    i->canvas, x, y, width, height, c
	);
	XConfigureWindow(_world->display(), (Window)i->canvas->id, cmask, &c);
    }
    i->Listen(i->cursensor == nil ? i->input : i->cursensor);
}

/*
 * Set toplevel windows' properties for ICCCM compatibility.
 */

void Scene::SetWindowProperties (
    Interactor* i, Coord x, Coord y, int width, int height, boolean placed
) {
    InteractorType itype = i->GetInteractorType();
    if (itype == InteriorInteractor || itype == PopupInteractor) {
	return;
    }

    WindowMgrHints wm;

    /* WM_NORMAL_HINTS */

    XSizeHints sizehints;
    wm.Size(i, x, y, width, height, placed, sizehints);

    /* WM_HINTS */

    Interactor* icon = i->GetIconInteractor();
    Canvas* dummycanvas = nil;
    Canvas*& iconcanvas = icon ? icon->canvas : dummycanvas;
    Interactor* leader = i->GetGroupLeader();
    Interactor* owner = i->GetTransientFor();
    if (leader == nil || leader->canvas == nil) {
	leader = owner;
    }
    Canvas* leadercanvas = leader ? leader->canvas : nil;
    wm.Compute(i, icon, iconcanvas, leader, leadercanvas);

    /* WM_CLASS */
    XClassHint classhint;
    wm.ClassHint(i, classhint);

    /* WM_ICON_NAME */
    const char* icon_name = i->GetIconName();
    if (icon_name == nil) {
	icon_name = classhint.res_name;
    }

    /* WM_NAME */
    const char* title = i->GetName();
    if (title == nil) {
	title = classhint.res_name;
    }

    /* WM_PROTOCOLS */
    /* we do not participate in any protocols */

    /* WM_COLORMAP_WINDOWS */
    /* we do not manipulate colormaps */

    Window w = (Window)i->canvas->id;
    WorldRep* rep = i->GetWorld()->Rep();
    char* clienthost = rep->hostname();
    switch (itype) {
	case ApplicationInteractor:
	    XSetCommand(rep->display(), w, saved_argv, saved_argc);
	    /* fall through */
	case ToplevelInteractor:
	    XSetIconName(rep->display(), w, icon_name);
	    /* fall through */
	case TransientInteractor:
	case IconInteractor:
	    XStoreName(rep->display(), w, title);
	    XChangeProperty(
		rep->display(), w, XA_WM_CLIENT_MACHINE, XA_STRING, 8,
		PropModeReplace, (unsigned char*)clienthost, strlen(clienthost)
	    );
	    XSetNormalHints(rep->display(), w, &sizehints);
	    wm.Set(rep->display(), w);
	    XSetClassHint(rep->display(), w, &classhint);
	    if (owner != nil && owner->canvas != nil) {
		XSetTransientForHint(
		    rep->display(), w, (Window)owner->canvas->id
		);
	    }
	    break;
    }
}

/*
 * Map an interactor and wait for confirmation of its position.
 * The interactor is either being placed by the user or inserted
 * in the world (in which case a window manager might choose to allocate
 * it a different size and placement).
 */

struct EventInfo {
    Window w;
    Interactor* i;
};

static Bool MapOrRedraw (Display*, register XEvent* xe, char* w) {
    EventInfo* x = (EventInfo*)w;
    return (
	/* target window is mapped */
	(xe->type == MapNotify && xe->xmap.window == x->w) ||

	/* target window is configued */
	(xe->type == ConfigureNotify && xe->xconfigure.window == x->w) ||

	/* a window other than the target is exposed */
	(xe->type == Expose && xe->xexpose.window != x->w &&
	    _world->itable()->Find(x->i, (void*)xe->xexpose.window)
	)
    );
}

void Scene::DoMap (Interactor* i, int w, int h) {
    EventInfo info;
    XEvent xe;
    boolean assigned = false;
    Display* d = i->GetWorld()->Rep()->display();
    info.w = (Window)i->canvas->id;
    XMapRaised(d, info.w);
    for (;;) {
	XIfEvent(d, &xe, MapOrRedraw, (char*)&info);
	if (xe.type == MapNotify) {
	    if (!assigned) {
		Assign(i, 0, 0, w, h);
	    }
	    i->SendActivate();
	    break;
	} else if (xe.type == ConfigureNotify) {
	    i->SendResize(
		xe.xconfigure.x, xe.xconfigure.y,
		xe.xconfigure.width, xe.xconfigure.height
	    );
	    assigned = true;
	} else if (xe.type == Expose) {
	    info.i->SendRedraw(
		xe.xexpose.x, xe.xexpose.y,
		xe.xexpose.width, xe.xexpose.height, xe.xexpose.count
	    );
	}
    }
    i->canvas->status = CanvasMapped;
}

void Scene::Map (Interactor* i, boolean raised) {
    Window w = (Window)i->canvas->id;
    Display* d = i->GetWorld()->Rep()->display();
    if (raised) {
	XMapRaised(d, w);
    } else {
	XMapWindow(d, w);
    }
    i->canvas->status = CanvasMapped;
}

/*
 * We must send a synthetic UnMapNotify for ICCCM compatibility
 */

void Scene::Unmap (Interactor* i) {
    WorldRep* rep = GetWorld()->Rep();
    Window w = (Window)i->canvas->id;
    XUnmapWindow(rep->display(), w);
    i->canvas->status = CanvasUnmapped;
    if (parent == nil && i->GetInteractorType() != PopupInteractor) {
	Bool propagate = False;
	Mask eventmask = (SubstructureRedirectMask|SubstructureNotifyMask);
	XEvent xe;

	xe.type = UnmapNotify;
	xe.xunmap.type = UnmapNotify;
	xe.xunmap.display = rep->display();
	xe.xunmap.event = rep->root();
	xe.xunmap.window = w;
	xe.xunmap.from_configure = False;

	XSendEvent(rep->display(), rep->root(), propagate, eventmask, &xe);
    }
}

void Scene::Raise (Interactor* i) {
    DoRaise(i);
    XRaiseWindow(i->GetWorld()->Rep()->display(), (Window)i->canvas->id);
}

void Scene::Lower (Interactor* i) {
    DoLower(i);
    XLowerWindow(i->GetWorld()->Rep()->display(), (Window)i->canvas->id);
}

void Scene::Move (Interactor* i, Coord x, Coord y, Alignment a) {
    Coord ax = x, ay = y;
    DoAlign(i, a, ax, ay);
    i->left = ax;
    i->bottom = ay;
    DoMove(i, ax, ay);
    XMoveWindow(
	i->GetWorld()->Rep()->display(), (Window)i->canvas->id,
	ax, ymax - ay - i->ymax
    );
    i->left = ax;
    i->bottom = ay;
}
