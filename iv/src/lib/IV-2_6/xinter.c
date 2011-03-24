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
 * X-specific code dealing with interactors and scenes.
 */

#include "../IV-X11/wtable.h"
#include <InterViews/canvas.h>
#include <InterViews/cursor.h>
#include <InterViews/display.h>
#include <InterViews/event.h>
#include <InterViews/font.h>
#include <InterViews/handler.h>
#include <InterViews/hit.h>
#include <InterViews/sensor.h>
#include <InterViews/world.h>
#include <InterViews/2.6/InterViews/interactor.h>
#include <InterViews/2.6/InterViews/iwindow.h>
#include <InterViews/2.6/InterViews/painter.h>
#include <InterViews/2.6/InterViews/scene.h>
#include <InterViews/2.6/InterViews/shape.h>
#include <IV-X11/Xlib.h>
#include <IV-X11/Xutil.h>
#include <IV-X11/xcanvas.h>
#include <IV-X11/xcursor.h>
#include <IV-X11/xdisplay.h>
#include <IV-X11/xevent.h>
#include <IV-X11/xwindow.h>
#include <OS/math.h>

void Interactor::request(Requisition& r) const {
    if (output == nil) {
	Interactor* i = (Interactor*)this;
	i->Config(World::current());
    }
    Display* d = GetWorld()->display();
    float align = 0.0;
    Requirement rx(
	d->to_coord(shape->width),
	d->to_coord(shape->hstretch), d->to_coord(shape->hshrink), align
    );
    Requirement ry(
	d->to_coord(shape->height),
	d->to_coord(shape->vstretch), d->to_coord(shape->vshrink), align
    );
    r.require(Dimension_X, rx);
    r.require(Dimension_Y, ry);
}

/*
 * If an allocation changes, we must ensure that the subwindows
 * are destroyed so that the next draw will recreate them.
 */

void Interactor::allocate(Canvas*, const Allocation& a, Extension& ext) {
    ext.xy_extents(a);
    if (window != nil && window->bound()) {
	WindowRep* w = window->rep();
	XDestroyWindow(w->display_->rep()->display_, w->xwindow_);
	window->unbind();
    }
}

/*
 * Draw a damaged area of the interactor's canvas.
 * Since interactors have their own subwindows, we don't need
 * to draw anything here.  However, we need to check to see
 * if we need to allocate the subwindow because the outer window
 * might have been unbound without our knowing.  We can't handle this
 * in allocate because our allocation might not have changed.
 */

void Interactor::draw(Canvas* c, const Allocation& a) const {
    if (window != nil && window->bound()) {
	/* already allocated */
	return;
    }
    Interactor* i = (Interactor*)this;
    Window* cw = c->window();
    Display* d = cw->rep()->display_;
    if (i->window == nil) {
	i->window = new InteractorWindow(i, cw);
	i->canvas = window->canvas();
    }
    i->window->display(d);
    i->window->double_buffered(false);

    CanvasRep* cr = i->canvas->rep();
    const Allotment& ax = a.allotment(Dimension_X);
    const Allotment& ay = a.allotment(Dimension_Y);
    cr->width_ = ax.span();
    cr->height_ = ay.span();
    cr->pwidth_ = d->to_pixels(cr->width_);
    cr->pheight_ = d->to_pixels(cr->height_);

    WindowRep* w = i->window->Window::rep();
    w->xpos_ = d->to_pixels(ax.origin());
    w->ypos_ = c->rep()->pheight_ - d->to_pixels(ay.origin()) - cr->pheight_;

    i->window->bind();
    i->xmax = cr->pwidth_ - 1;
    i->ymax = cr->pheight_ - 1;
    cr->status_ = Canvas::mapped;
    i->Resize();
    XMapRaised(d->rep()->display_, w->xwindow_);
}

/*
 * Determine a handler for an interactor window.
 * This should simply end up calling interactor_->pick and returning
 * the appropriate InteractorHandler.
 */

Handler* InteractorWindow::target(const Event& e) const {
    if (!e.rep()->has_pointer_location()) {
	return nil;
    }

    WindowRep* w = Window::rep();
    Hit h(&e);
    w->glyph_->pick(w->canvas_, w->allocation_, 0, h);
    return h.handler();
}

/*
 * Set a handler for the interactor if the event is of interest.
 * This is complicated by grabbing, which occurs on mouse down events.
 * The case we want to make sure to when the UpEvent is sent
 * to an uninterested interactor.
 */

class InteractorHandler : public Handler {
public:
    InteractorHandler(Interactor*);
    virtual ~InteractorHandler();

    virtual void event(Event&);
private:
    Interactor* interactor_;
};

static boolean grabbing;

void Interactor::pick(Canvas*, const Allocation&, int depth, Hit& h) {
    const Event* ep = h.event();
    if (ep != nil) {
	Event& e = *(Event*)ep;
	e.GetInfo();
	Sensor* s = cursensor == nil ? input : cursensor;
	if ((s != nil && s->Caught(e)) || grabbing) {
	    e.target = this;
	    e.y = ymax - e.y;
	    if (e.eventType == DownEvent) {
		grabbing = true;
	    } else if (e.eventType == UpEvent) {
		grabbing = false;
	    }
	    h.target(depth, this, 0, new InteractorHandler(this));
	}
    }
}

InteractorHandler::InteractorHandler(Interactor* i) {
    interactor_ = i;
}

InteractorHandler::~InteractorHandler() { }

/*
 * Re-filter the event here before calling Handle because
 * the target might be associated with a grab.  Note that this filtering
 * will not occur when some object other than the window reads events,
 * typically calling e.target->Handle(e).  Interactors really ought
 * to learn to grab, but it isn't worth trying to change them.
 */

void InteractorHandler::event(Event& e) {
    Interactor* i = interactor_;
    Sensor* s = i->cursensor == nil ? i->input : i->cursensor;
    if (s != nil && s->Caught(e)) {
	i->Handle(e);
    }
}

void Interactor::Listen(Sensor* s) {
    cursensor = s;
    if (window == nil) {
	/* can't set input interest without window */
	return;
    }
    Mask m = ExposureMask;
    if (s != nil) {
	m |= s->mask;
    }
    WindowRep* w = window->rep();
    XSelectInput(w->display_->rep()->display_, w->xwindow_, m);
}

int Interactor::CheckQueue() { return QLength(window->rep()->dpy()); }

void Interactor::GetPosition(IntCoord& left, IntCoord& bottom) const {
    if (window == nil) {
	/* try to cause an error */
	left = 32767;
	bottom = 32767;
	return;
    }
    WindowRep* w = window->rep();
    Display* d = w->display_;
    XDisplay* dpy = d->rep()->display_;

    int x, y;
    XWindow child;
    XTranslateCoordinates(
	dpy, w->xwindow_, d->rep()->root_, 0, 0, &x, &y, &child
    );
    left = x;
    bottom = d->pheight() - y - window->pheight();
}

InteractorWindow::InteractorWindow(Interactor* i) : Window(i) {
    interactor_ = i;
    parent_ = nil;
}

InteractorWindow::InteractorWindow(Interactor* i, Window* w) : Window(i) {
    interactor_ = i;
    parent_ = w;
}

InteractorWindow::~InteractorWindow() { }

void InteractorWindow::set_attributes() {
    WindowRep* w = Window::rep();
    w->xattrmask_ |= CWBackPixmap;
    w->xattrs_.background_pixmap = ParentRelative;

    w->xattrmask_ |= CWWinGravity;
    w->xattrs_.win_gravity = UnmapGravity;

    w->xattrmask_ |= CWEventMask;
    Mask m = ExposureMask;
    Sensor* s = interactor_->cursensor;
    if (s == nil) {
	s = interactor_->input;
	interactor_->cursensor = s;
    }
    if (s != nil) {
	m |= s->mask;
    }
    w->xattrs_.event_mask = m;

    if (interactor_->cursor_ != nil) {
	w->xattrmask_ |= CWCursor;
	w->xattrs_.cursor = interactor_->cursor_->rep()->xid(w->display_);
    }

    switch (interactor_->canvas_type_) {
    case CanvasShapeOnly:
    case CanvasInputOutput:
	break;
    case CanvasInputOnly:
	w->xclass_ = InputOnly;
	break;
    case CanvasSaveUnder:
	save_under(true);
	break;
    case CanvasSaveContents:
	save_contents(true);
	break;
    case CanvasSaveBoth:
	save_under(true);
	save_contents(true);
    }
}

void InteractorWindow::bind() {
    if (parent_ == nil) {
	parent_ = interactor_->parent->window;
    }

    WindowRep* w = Window::rep();
    WindowRep* pw = parent_->Window::rep();
    CanvasRep* c = canvas()->rep();
    w->toplevel_ = pw->toplevel_;
    w->do_bind(this, pw->xwindow_, w->xpos_, w->ypos_);
    w->init_renderer(this);
}

void InteractorWindow::unbind() {
    Window::unbind();
    interactor_->Orphan();
}

void InteractorWindow::receive(const Event& e) {
    int ymax = pheight() - 1;
    int itop;
    XEvent& xe = e.rep()->xevent_;
    switch (xe.type) {
    case Expose:
	itop = ymax - xe.xexpose.y;
	interactor_->Redraw(
	    xe.xexpose.x, itop - xe.xexpose.height + 1,
	    xe.xexpose.x + xe.xexpose.width - 1, itop
	);
	break;
    case GraphicsExpose:
	itop = ymax - xe.xgraphicsexpose.y;
	interactor_->Redraw(
	    xe.xgraphicsexpose.x, itop - xe.xgraphicsexpose.height + 1,
	    xe.xgraphicsexpose.x + xe.xgraphicsexpose.width - 1, itop
	);
	break;
    case MotionNotify:
	e.rep()->acknowledge_motion();
	break;
    }
}

/*
 * Interactor-specifc anachronistic World member functions
 */

static void AlignPosition(Window* w, Alignment a) {
    float xalign = 0.0, yalign = 0.0;
    boolean needs_align = true;
    switch (a) {
    case BottomRight:
    case Right:
	xalign = 1.0;
	break;
    case Top:
    case TopLeft:
	yalign = 1.0;
	break;
    case BottomCenter:
    case HorizCenter:
	xalign = 0.5;
	break;
    case CenterLeft:
    case VertCenter:
	yalign = 0.5;
	break;
    case TopCenter:
	xalign = 0.5; yalign = 1.0;
	break;
    case TopRight:
	xalign = 1.0; yalign = 1.0;
	break;
    case _lib_iv2_6(Center):
	xalign = 0.5; yalign = 0.5;
	break;
    case CenterRight:
	xalign = 1.0; yalign = 0.5;
	break;
    case Bottom:
    case Left:
    case BottomLeft:
	needs_align = false;
	break;
    }
    if (needs_align) {
	w->align(xalign, yalign);
    }
}

void World::InsertApplication(Interactor* i) {
    delete i->insert_window;
    ApplicationWindow* w = new ApplicationWindow(i);
    i->insert_window = w;
    i->managed_window = w;
    w->display(display_);
    w->map();
    Handler* h = new InteractorHandler(i);
    w->focus_event(h, h);
}

void World::InsertApplication(
    Interactor* i, IntCoord left, IntCoord bottom, Alignment a
) {
    delete i->insert_window;
    ApplicationWindow* w = new ApplicationWindow(i);
    i->insert_window = w;
    i->managed_window = w;
    w->display(display_);
    w->pplace(left, bottom);
    AlignPosition(w, a);
    w->map();
    Handler* h = new InteractorHandler(i);
    w->focus_event(h, h);
}

void World::InsertToplevel(Interactor* i, Interactor* leader) {
    delete i->insert_window;
    TopLevelWindow* w = new TopLevelWindow(i);
    i->insert_window = w;
    i->managed_window = w;
    w->display(display_);
    Window* g;
    if (leader == i) {
	g = w;
    } else {
	g = leader->window;
    }
    w->group_leader(g);
    w->map();
    Handler* h = new InteractorHandler(i);
    w->focus_event(h, h);
}

void World::InsertToplevel(
    Interactor* i, Interactor* leader, IntCoord left, IntCoord bottom,
    Alignment a
) {
    delete i->insert_window;
    TopLevelWindow* w = new TopLevelWindow(i);
    i->insert_window = w;
    i->managed_window = w;
    w->display(display_);
    w->pplace(left, bottom);
    AlignPosition(w, a);
    Window* g;
    if (leader == i) {
	g = w;
    } else {
	g = leader->window;
    }
    w->group_leader(g);
    w->map();
    Handler* h = new InteractorHandler(i);
    w->focus_event(h, h);
}

void World::InsertPopup(Interactor* i) {
    delete i->insert_window;
    Window* w = new PopupWindow(i);
    i->insert_window = w;
    i->managed_window = nil;
    w->display(display_);
    w->map();
}

void World::InsertPopup(
    Interactor* i, IntCoord left, IntCoord bottom, Alignment a
) {
    delete i->insert_window;
    Window* w = new PopupWindow(i);
    i->insert_window = w;
    i->managed_window = nil;
    w->display(display_);
    w->pplace(left, bottom);
    AlignPosition(w, a);
    w->map();
}

void World::InsertTransient(Interactor* i, Interactor* primary) {
    delete i->insert_window;
    TransientWindow* w = new TransientWindow(i);
    i->insert_window = w;
    i->managed_window = w;
    w->display(display_);
    Window* pw;
    if (primary == i) {
	pw = w;
    } else {
	pw = primary->window;
    }
    w->group_leader(pw);
    w->transient_for(pw);
    w->map();
    Handler* h = new InteractorHandler(i);
    w->focus_event(h, h);
}

void World::InsertTransient(
    Interactor* i, Interactor* primary, IntCoord left, IntCoord bottom,
    Alignment a
) {
    delete i->insert_window;
    TransientWindow* w = new TransientWindow(i);
    i->insert_window = w;
    i->managed_window = w;
    w->display(display_);
    w->pplace(left, bottom);
    AlignPosition(w, a);
    Window* pw;
    if (primary == i) {
	pw = w;
    } else {
	pw = primary->window;
    }
    w->group_leader(pw);
    w->transient_for(pw);
    w->map();
    Handler* h = new InteractorHandler(i);
    w->focus_event(h, h);
}

void World::InsertIcon(Interactor* i) {
    delete i->insert_window;
    IconWindow* w = new IconWindow(i);
    i->insert_window = w;
    i->managed_window = w;
    w->display(display_);
    w->map();
    Handler* h = new InteractorHandler(i);
    w->focus_event(h, h);
}

void World::InsertIcon(
    Interactor* i, IntCoord left, IntCoord bottom, Alignment a
) {
    delete i->insert_window;
    IconWindow* w = new IconWindow(i);
    i->insert_window = w;
    i->managed_window = w;
    w->display(display_);
    w->pplace(left, bottom);
    AlignPosition(w, a);
    w->map();
    Handler* h = new InteractorHandler(i);
    w->focus_event(h, h);
}

void World::Move(Interactor* i, IntCoord x, IntCoord y) {
    if (i->window != nil) {
	i->window->move(display_->to_coord(x), display_->to_coord(y));
    }
}

void World::Raise(Interactor* i) {
    if (i->window != nil) {
	i->window->raise();
    }
}

void World::Lower(Interactor* i) {
    if (i->window != nil) {
	i->window->lower();
    }
}

void World::Change(Interactor* i) {
    if (i->window != nil) {
	WindowRep* w = i->window->rep();
	XWindow xw = w->xwindow_;
	if (xw != CanvasRep::unbound) {
	    CanvasRep* c = i->canvas->rep();
	    Shape* s = i->GetShape();
	    if (c->pwidth_ != s->width || c->pheight_ != s->height) {
		i->insert_window->resize();
	    } else {
		i->Resize();
	    }
	}
    }
}

void World::Remove(Interactor* i) {
    i->parent = nil;
    if (i->insert_window != nil) {
	delete i->insert_window;
	i->insert_window = nil;
	i->managed_window = nil;
    }
    if (i->window != nil) {
	/* implicit i->Orphan() from InteractorWindow::unbind */
	i->window->unbind();
	i->Deactivate();
    }
}

/*
 * Scene operations for manipulating interactor windows.
 */

void Scene::Place(
    Interactor* i, IntCoord l, IntCoord b, IntCoord r, IntCoord t, boolean map
) {
    IntCoord x = l;
    IntCoord y = ymax - t;
    unsigned int width = r - l + 1;
    unsigned int height = t - b + 1;
    if (width == 0) {
	width = Math::round(inch);
    }
    if (height == 0){ 
	height = Math::round(inch);
    }

    Display* d = window->display();
    InteractorWindow* iw = i->window;
    XWindow old_xwindow;
    if (iw == nil) {
	old_xwindow = WindowRep::unbound;
	iw = new InteractorWindow(i, canvas->window());
	i->window = iw;
	i->canvas = iw->canvas();
    } else if (iw->bound()) {
	old_xwindow = iw->Window::rep()->xwindow_;
    }
    iw->display(d);
    iw->double_buffered(false);

    WindowRep* w = iw->Window::rep();
    CanvasRep* c = i->canvas->rep();
    w->xpos_ = x;
    w->ypos_ = y;
    c->pwidth_ = width;
    c->pheight_ = height;
    c->width_ = d->to_coord(width);
    c->height_ = d->to_coord(height);

    iw->bind();
    i->xmax = width - 1;
    i->ymax = height - 1;
    c->status_ = Canvas::mapped;
    i->Resize();

    XDisplay* dpy = d->rep()->display_;
    if (old_xwindow != WindowRep::unbound) {
	XDestroyWindow(dpy, old_xwindow);
    }
    if (map) {
	XMapRaised(dpy, w->xwindow_);
    }
}

void Scene::Map(Interactor* i, boolean raised) {
    if (window == nil || !window->bound() || i->window == nil) {
	return;
    }
    WindowRep* w = i->window->rep();
    XDisplay* dpy = w->display_->rep()->display_;
    XWindow xw = w->xwindow_;
    if (raised) {
	XMapRaised(dpy, xw);
    } else {
	XMapWindow(dpy, xw);
    }
    i->canvas->rep()->status_ = Canvas::mapped;
}

void Scene::Unmap(Interactor* i) {
    if (window == nil || !window->bound() || i->window == nil) {
	return;
    }
    WindowRep* w = i->window->rep();
    XUnmapWindow(w->display_->rep()->display_, w->xwindow_);
    i->canvas->rep()->status_ = Canvas::unmapped;
}

void Scene::Move(Interactor* i, IntCoord x, IntCoord y, Alignment a) {
    if (window == nil || !window->bound() || i->window == nil) {
	return;
    }
    IntCoord ax = x, ay = y;
    DoAlign(i, a, ax, ay);
    DoMove(i, ax, ay);
    Display* d = window->rep()->display_;
    i->window->move(d->to_coord(ax), d->to_coord(ay));
}
