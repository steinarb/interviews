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
 * X11 window manager hints
 */

#include <InterViews/bitmap.h>
#include <InterViews/canvas.h>
#include <InterViews/interactor.h>
#include <InterViews/shape.h>
#include <InterViews/world.h>
#include <InterViews/X11/wmhints.h>
#include <InterViews/X11/Xlib.h>
#include <InterViews/X11/Xutil.h>
#include <X11/Xatom.h>

WindowMgrHints::WindowMgrHints() {
    wm = new XWMHints;
}

WindowMgrHints::WindowMgrHints(XWMHints* x) {
    wm = x;
}

WindowMgrHints::WindowMgrHints(Display* d, Window w) {
    wm = XGetWMHints(d, w);
}

WindowMgrHints::~WindowMgrHints() {
    delete wm;
}

void WindowMgrHints::Set(Display* d, Window w) {
    XSetWMHints(d, w, wm);
}

void WindowMgrHints::Compute(
    Interactor* i, Interactor* icon, Canvas*& iconcanvas,
    Interactor* leader, Canvas* leadercanvas
) {
    wm->flags = 0;

    wm->flags |= InputHint;
    wm->input = True;

    wm->flags |= StateHint;
    wm->initial_state = i->GetStartIconic() ? IconicState : NormalState;

    IconPixmap(i->GetIconBitmap());
    IconWindow(i, icon, iconcanvas);

    const char* g = i->GetIconGeometry();
    if (g == nil && icon != nil) {
	g = icon->GetGeometry();
    }
    IconPosition(i, g);

    IconMask(i->GetIconMask());
    WindowGroup(leader, leadercanvas);
}

void WindowMgrHints::WindowGroup(Interactor* leader, Canvas* leadercanvas) {
    if (leader != nil && leadercanvas != nil) {
	wm->flags |= WindowGroupHint;
	wm->window_group = (Window)leadercanvas->Id();
    } else {
	wm->flags &= ~WindowGroupHint;
	wm->window_group = None;
    }
}

void WindowMgrHints::IconPixmap(Bitmap* bitmap) {
    if (bitmap != nil && bitmap->Map() != nil) {
	wm->flags |= IconPixmapHint;
	wm->icon_pixmap = (Pixmap)bitmap->Map();
    } else {
	wm->flags &= ~IconPixmapHint;
	wm->icon_pixmap = None;
    }
}

void WindowMgrHints::IconMask(Bitmap* mask) {
    if (mask != nil && mask->Map() != nil) {
	wm->flags |= IconMaskHint;
	wm->icon_mask = (Pixmap)mask->Map();
    } else {
	wm->flags &= ~IconMaskHint;
	wm->icon_mask = None;
    }
}

void WindowMgrHints::IconWindow(
    Interactor* i, Interactor* icon, Canvas*& iconcanvas
) {
    if (icon != nil) {
	i->PlaceIcon(icon, iconcanvas);
	wm->flags |= IconWindowHint;
	wm->icon_window = (Window)iconcanvas->Id();
    } else {
	wm->flags &= ~IconWindowHint;
	wm->icon_window = None;
    }
}

void WindowMgrHints::IconPosition(Interactor* i, const char* g) {
    wm->flags &= ~IconPositionHint;
    if (g != nil) {
	Coord x = 0;
	Coord y = 0;
	unsigned int w = i->GetShape()->width;
	unsigned int h = i->GetShape()->height;
	Bitmap* b = i->GetIconBitmap();
	if (b != nil) {
	    w = b->Width();
	    h = b->Height();
	}
	Interactor* icon = i->GetIconInteractor();
	if (icon != nil) {
	    w = icon->GetShape()->width;
	    h = icon->GetShape()->height;
	}

	World* world = i->GetWorld();
	unsigned r = world->ParseGeometry(g, x, y, w, h);
	if ((r & GeomXNegative) != 0) {
	    x = world->Width() + x - w;
	}
	if ((r & GeomYNegative) != 0) {
	    y = world->Height() + y - h;
	}
	if ((r & (GeomXValue|GeomYValue)) != 0) {
	    wm->flags |= IconPositionHint;
	    wm->icon_x = x;
	    wm->icon_y = y;
	}
    }
}

int WindowMgrHints::Squeeze(register int a, int lower, int upper) {
    if (a < lower) {
	return lower;
    }
    if (a > upper) {
	return upper;
    }
    return a;
}

void WindowMgrHints::Size(
    Interactor* i, Coord x, Coord y, int width, int height, boolean placed,
    XSizeHints& sizehints
) {
    sizehints.flags = 0;

    const int MINSIZE = 1;
    const int BIGSIZE = 32123;
    Shape* s = i->GetShape();

    if (placed) {
	sizehints.flags |= USPosition | USSize;
    } else {
	sizehints.flags |= PSize;
    }
    sizehints.x = x;
    sizehints.y = y;
    sizehints.width = width;
    sizehints.height = height;

    sizehints.flags |= PMinSize;
    sizehints.min_width = Squeeze(s->width - s->hshrink, MINSIZE, BIGSIZE);
    sizehints.min_height = Squeeze(s->height - s->vshrink, MINSIZE, BIGSIZE);

    sizehints.flags |= PMaxSize;
    sizehints.max_width = Squeeze(s->width + s->hstretch, MINSIZE, BIGSIZE);
    sizehints.max_height = Squeeze(s->height + s->vstretch, MINSIZE, BIGSIZE);

    sizehints.flags |= PResizeInc;
    sizehints.width_inc = min(s->hunits, 1);
    sizehints.height_inc = min(s->vunits, 1);

    if (s->aspect == 1) {
	sizehints.flags |= PAspect;
	sizehints.min_aspect.x = 1;
	sizehints.min_aspect.y = 1;
	sizehints.max_aspect.x = 1;
	sizehints.max_aspect.y = 1;
    }
}

void WindowMgrHints::ClassHint(Interactor* i, XClassHint& classhint) {
    classhint.res_name = (char*)i->GetInstance();
    if (classhint.res_name == nil) {
	classhint.res_name = (char*)i->GetWorld()->GetInstance();
	if (classhint.res_name == nil) {
	    classhint.res_name = "unnamed";
	}
    }

    classhint.res_class = (char*)i->GetClassName();
    if (classhint.res_class == nil) {
	classhint.res_class = (char*)i->GetWorld()->GetClassName();
	if (classhint.res_class == nil) {
	    classhint.res_class = "Unnamed";
	}
    }
}
