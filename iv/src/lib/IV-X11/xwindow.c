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

#include "damagelist.h"
#include "wtable.h"
#include <InterViews/bitmap.h>
#include <InterViews/canvas.h>
#include <InterViews/color.h>
#include <InterViews/cursor.h>
#include <InterViews/display.h>
#include <InterViews/event.h>
#include <InterViews/handler.h>
#include <InterViews/glyph.h>
#include <InterViews/hit.h>
#include <InterViews/session.h>
#include <InterViews/style.h>
#include <InterViews/window.h>
#include <IV-X11/Xlib.h>
#include <IV-X11/Xutil.h>
#include <IV-X11/xbitmap.h>
#include <IV-X11/xcanvas.h>
#include <IV-X11/xcursor.h>
#include <IV-X11/xdisplay.h>
#include <IV-X11/xevent.h>
#include <IV-X11/xwindow.h>
#include <OS/host.h>
#include <OS/math.h>
#include <OS/string.h>
#include <X11/Xatom.h>

Window::Window(Glyph* g) {
    WindowRep* w = new WindowRep;
    rep_ = w;
    w->glyph_ = g;
    w->glyph_->ref();
    w->display_ = nil;
    w->left_ = 0;
    w->bottom_ = 0;
    w->focus_in_ = nil;
    w->focus_out_ = nil;
    w->wm_delete_ = nil;
    w->xwindow_ = WindowRep::unbound;
    w->xattrmask_ = 0;
    w->xclass_ = InputOutput;
    w->double_buffered_ = true;
    w->clear_mapping_info();
    w->cursor_ = defaultCursor;
    w->toplevel_ = this;
    w->canvas_ = new Canvas;
    w->canvas_->rep()->window_ = this;
}

/*
 * Construct a window with a given representation object,
 * thereby overriding the default representation.
 */

Window::Window(WindowRep* w) {
    rep_ = w;
}

/*
 * The virtual call to unbind cannot be overridden by a subclass
 * because we are calling it from the destructor.
 */

Window::~Window() {
    Window::unbind();
    WindowRep* w = rep();
    Resource::unref(w->glyph_);
    Resource::unref(w->focus_in_);
    Resource::unref(w->focus_out_);
    Resource::unref(w->wm_delete_);
    delete w->canvas_;
    delete rep_;
    rep_ = nil;
}

void Window::display(Display* d) {
    WindowRep* w = rep();
    w->display_ = d;
    w->canvas_->rep()->display_ = d;
}

Display* Window::display() const {
    WindowRep* w = rep();
    return w->display_;
}

Canvas* Window::canvas() const {
    WindowRep* w = rep();
    return w->canvas_;
}

void Window::save_under(boolean b) {
    WindowRep* w = rep();
    w->xattrmask_ |= CWSaveUnder;
    w->xattrs_.save_under = b;
}

boolean Window::save_under() const {
    WindowRep* w = rep();
    return ((w->xattrmask_ & CWSaveUnder) != 0) && w->xattrs_.save_under;
}

void Window::save_contents(boolean b) {
    WindowRep* w = rep();
    w->xattrmask_ |= CWBackingStore;
    w->xattrs_.backing_store = b ? WhenMapped : NotUseful;
}

boolean Window::save_contents() const {
    WindowRep* w = rep();
    return ((w->xattrmask_ & CWBackingStore) != 0) &&
	w->xattrs_.backing_store != NotUseful;
}

void Window::double_buffered(boolean b) {
    rep()->double_buffered_ = b;
}

boolean Window::double_buffered() const { return rep()->double_buffered_; }

void Window::cursor(Cursor* c) {
    WindowRep* w = rep();
    w->cursor_ = c;
    XWindow xw = w->xwindow_;
    if (xw != WindowRep::unbound) {
	XDisplay* dpy = w->dpy();
	if (c == nil) {
	    XUndefineCursor(dpy, xw);
	} else {
	    XDefineCursor(dpy, xw, c->rep()->xid(w->display_));
	}
	XFlush(dpy);
    }
}

Cursor* Window::cursor() const { return rep()->cursor_; }

void Window::place(Coord left, Coord bottom) {
    WindowRep* w = rep();
    w->placed_ = true;
    w->left_ = left;
    w->bottom_ = bottom;
}

void Window::pplace(IntCoord pleft, IntCoord pbottom) {
    WindowRep* w = rep();
    Display* d = w->display_;
    w->placed_ = true;
    w->left_ = d->to_coord(pleft);
    w->bottom_ = d->to_coord(pbottom);
}

void Window::align(float x, float y) {
    WindowRep* w = rep();
    w->aligned_ = true;
    w->xalign_ = x;
    w->yalign_ = y;
}

Coord Window::left() const {
    WindowRep* w = rep();
    Display* d = w->display_;
    if (d == nil) {
	return w->left_;
    }
    w->check_position(this);
    return d->to_coord(w->xpos_);
}

Coord Window::bottom() const {
    WindowRep* w = rep();
    Display* d = w->display_;
    if (d == nil) {
	return w->bottom_;
    }
    w->check_position(this);
    return d->height() - d->to_coord(w->ypos_) - height();
}

Coord Window::width() const { return rep()->canvas_->width(); }
Coord Window::height() const { return rep()->canvas_->height(); }
unsigned int Window::pwidth() const { return rep()->canvas_->pwidth(); }
unsigned int Window::pheight() const { return rep()->canvas_->pheight(); }

void Window::map() {
    if (display() == nil) {
	display(Session::instance()->default_display());
    }
    configure();
    default_geometry();
    compute_geometry();
    bind();
    set_props();
    do_map();
}

void Window::configure() { }

void Window::default_geometry() {
    WindowRep* w = rep();
    Display* d = w->display_;
    w->glyph_->request(w->shape_);
    Coord width = w->shape_.requirement(Dimension_X).natural();
    Coord height = w->shape_.requirement(Dimension_Y).natural();
    w->canvas_->size(width, height);
    w->xpos_ = d->to_pixels(w->left_);
    w->ypos_ = d->pheight() - d->to_pixels(w->bottom_) - pheight();
    if (w->aligned_) {
	w->xpos_ -= d->to_pixels(w->xalign_ * width);
	w->ypos_ += d->to_pixels(w->yalign_ * height);
    }
}

void Window::compute_geometry() { }

/*
 * Bind the current window description to its X counterpart, creating
 * the X window if necessary.
 */

void Window::bind() {
    WindowRep* w = rep();
    w->do_bind(this, w->display_->rep()->root_, w->xpos_, w->ypos_);
}

/*
 * Unbind the window from its X counterpart, removing the window
 * from the xid -> window table and making sure it is not on
 * the damage list.  X will destroy subwindows automatically,
 * so there is no need to destroy the window if it has a parent.
 * In fact, it will cause an error if the windows are destroyed
 * out of order (parent destroyed before children).
 */

void Window::unbind() {
    WindowRep* w = rep();
    Display* d = w->display_;
    if (d != nil && w->xwindow_ != WindowRep::unbound) {
	DisplayRep* r = d->rep();
	if (w->toplevel_ == this) {
	    XDestroyWindow(r->display_, w->xwindow_);
	}
	r->wtable_->remove(w->xwindow_);
	DamageList& list = *(r->damage_list_);
	for (int i = 0; i < list.count(); i++) {
	    if (list.item(i) == this) {
		list.remove(i);
		break;
	    }
	}
    }
    w->xwindow_ = WindowRep::unbound;
    w->clear_mapping_info();
    CanvasRep* c = w->canvas_->rep();
    c->unbind();
    c->clear_damage();
}

boolean Window::bound() const {
    WindowRep* w = rep();
    Window* t = w->toplevel_;
    return (
	t != nil && w->xwindow_ != WindowRep::unbound &&
	t->rep()->xwindow_ == w->xtoplevel_
    );
}

void Window::set_attributes() {
    WindowRep* w = rep();

    w->xattrmask_ |= CWBackPixmap;
    w->xattrs_.background_pixmap = None;

    /*
     * It is necessary to set the border pixel to avoid trying
     * to use the parent's border.  The problem is the parent
     * might have a different visual.  Of course, none of this really
     * matters because we always use a border width of zero!
     */
    w->xattrmask_ |= CWBorderPixel;
    w->xattrs_.border_pixel = 0;

    w->xattrmask_ |= CWEventMask;
    w->xattrs_.event_mask = (
	KeyPressMask | KeyReleaseMask |
	ButtonPressMask | ButtonReleaseMask |
	PointerMotionMask | PointerMotionHintMask |
	ExposureMask |
	StructureNotifyMask |
	FocusChangeMask |
	OwnerGrabButtonMask
    );

    /*
     * These events are caught at the top-level and not propagated
     * out to the root window (meaning the window manager).
     */
    w->xattrmask_ |= CWDontPropagate;
    w->xattrs_.do_not_propagate_mask = (
	KeyPressMask | KeyReleaseMask |
	ButtonPressMask | ButtonReleaseMask | PointerMotionMask
    );

    w->xattrmask_ |= CWColormap;
    w->xattrs_.colormap = w->display_->rep()->cmap_;

    if (w->cursor_ != nil) {
	w->xattrmask_ |= CWCursor;
	w->xattrs_.cursor = w->cursor_->rep()->xid(w->display_);
    }
}

void Window::set_props() { }

void Window::do_map() {
    WindowRep* w = rep();
    XMapRaised(w->dpy(), w->xwindow_);
}

void Window::unmap() {
    WindowRep* w = rep();
    XUnmapWindow(w->dpy(), w->xwindow_);
    /* XSendEvent for ICCCM UnmapNotify? */
    unbind();
}

/*
 * Look at an event that has been received for this window.
 * Here we handle map/expose/configure events.
 */

void Window::receive(const Event& e) {
    WindowRep* w = rep();
    XEvent& xe = e.rep()->xevent_;
    switch (xe.type) {
    case MapNotify:
	w->map_notify(this, xe.xmap);
	break;
    case UnmapNotify:
	w->unmap_notify(this, xe.xunmap);
	break;
    case Expose:
	w->expose(this, xe.xexpose);
	break;
    case ConfigureNotify:
	w->configure_notify(this, xe.xconfigure);
	break;
    case MotionNotify:
	/* allow next pointer motion */
	e.rep()->acknowledge_motion();
	break;
    }
}

/*
 * Search for a handler for the given event.
 * For events that have no associated pointer location, return nil.
 * Otherwise, use pick on the glyph to find a handler.
 */

Handler* Window::target(const Event& e) const {
    WindowRep* w = rep();
    XEvent& xe = e.rep()->xevent_;
    switch (xe.type) {
    case FocusIn:
	return w->focus_in_;
    case FocusOut:
	return w->focus_out_;
    case ClientMessage:
	if (xe.xclient.data.l[0] == w->wm_delete_atom()) {
	    Handler* h = w->wm_delete_;
	    if (h == nil) {
		Session::instance()->quit();
	    }
	    return h;
	}
	return nil;
    default:
	if (!e.rep()->has_pointer_location()) {
	    return nil;
	}
	break;
    }

    Hit hit(&e);
    w->glyph_->pick(w->canvas_, w->allocation_, 0, hit);

    Handler* h = hit.handler();
    if (h != nil && (e.grabber() == nil || e.is_grabbing(h))) {
	return h;
    }

    return nil;
}

void Window::grab_pointer(Cursor* c) const {
    WindowRep* w = rep();
    XGrabPointer(
	w->dpy(), w->xwindow_, True, (unsigned int)w->xattrs_.event_mask,
	/* pointer_mode */ GrabModeAsync, /* keyboard_mode */ GrabModeAsync,
	/* confine_to */ None, c == nil ? None : c->rep()->xid(w->display_),
	CurrentTime
    );
}

void Window::ungrab_pointer() const {
    WindowRep* w = rep();
    XUngrabPointer(w->dpy(), CurrentTime);
}

void Window::repair() {
    WindowRep* w = rep();
    CanvasRep* c = w->canvas_->rep();
    if (c->start_repair()) {
	w->glyph_->draw(w->canvas_, w->allocation_);
	c->finish_repair();
    }
}

void Window::raise() {
    WindowRep* w = rep();
    XRaiseWindow(w->dpy(), w->xwindow_);
}

void Window::lower() {
    WindowRep* w = rep();
    XLowerWindow(w->dpy(), w->xwindow_);
}

void Window::move(Coord left, Coord bottom) {
    WindowRep* w = rep();
    Display* d = w->display_;
    XMoveWindow(
	d->rep()->display_, w->xwindow_,
	d->to_pixels(left), d->pheight() - d->to_pixels(bottom) - pheight()
    );
}

void Window::resize() {
    WindowRep* w = rep();
    CanvasRep* c = w->canvas_->rep();
    XResizeWindow(
	w->dpy(), w->xwindow_, c->pwidth_, c->pheight_
    );
    w->needs_resize_ = true;
}

/** class ManagedWindow **/

ManagedWindow::ManagedWindow(Glyph* g) : Window(g) {
    ManagedWindowRep* w = new ManagedWindowRep;
    rep_ = w;
    w->name_ = nil;
    w->geometry_ = nil;
    w->group_leader_ = nil;
    w->transient_for_ = nil;
    w->icon_name_ = nil;
    w->icon_geometry_ = nil;
    w->icon_ = nil;
    w->icon_bitmap_ = nil;
    w->icon_mask_ = nil;
    w->iconic_ = false;
}

ManagedWindow::~ManagedWindow() {
    ManagedWindowRep* w = rep_;
    delete w->name_;
    delete w->geometry_;
    delete w->icon_name_;
    delete w->icon_geometry_;
    delete w;
}

Display* ManagedWindow::display() const { return Window::display(); }
const String* ManagedWindow::name() const { return rep()->name_; }
const String* ManagedWindow::geometry() const { return rep()->geometry_; }
const String* ManagedWindow::icon_name() const { return rep()->icon_name_; }
ManagedWindow* ManagedWindow::icon() const { return rep()->icon_; }
const String* ManagedWindow::icon_geometry() const {
    return rep()->icon_geometry_;
}
Bitmap* ManagedWindow::icon_bitmap() const { return rep()->icon_bitmap_; }
Bitmap* ManagedWindow::icon_mask() const { return rep()->icon_mask_; }
boolean ManagedWindow::iconic() const { return rep()->iconic_; }

void ManagedWindow::display(Display* d) {
    Window::display(d);
    ManagedWindowRep* w = rep();
    if (w->name_ == nil) {
	w->name_ = new CopyString(Session::instance()->name());
    }
}

void ManagedWindow::name(const String& s) {
    ManagedWindowRep* w = rep();
    w->name_ = new CopyString(s);
    w->do_set(this, &ManagedWindowRep::set_name);
}

void ManagedWindow::name(const char* s) {
    ManagedWindowRep* w = rep();
    w->name_ = new CopyString(s);
    w->do_set(this, &ManagedWindowRep::set_name);
}

void ManagedWindow::geometry(const String& g) {
    ManagedWindowRep* w = rep();
    w->geometry_ = new CopyString(g);
    w->do_set(this, &ManagedWindowRep::set_geometry);
}

void ManagedWindow::geometry(const char* g) {
    ManagedWindowRep* w = rep();
    w->geometry_ = new CopyString(g);
    w->do_set(this, &ManagedWindowRep::set_geometry);
}

void ManagedWindow::icon_name(const String& s) {
    ManagedWindowRep* w = rep();
    w->icon_name_ = new CopyString(s);
    w->do_set(this, &ManagedWindowRep::set_icon_name);
}

void ManagedWindow::icon_name(const char* s) {
    ManagedWindowRep* w = rep();
    w->icon_name_ = new CopyString(s);
    w->do_set(this, &ManagedWindowRep::set_icon_name);
}

void ManagedWindow::icon(ManagedWindow* i) {
    ManagedWindowRep* w = rep();
    w->icon_ = i;
    w->do_set(this, &ManagedWindowRep::set_icon);
}

void ManagedWindow::icon_geometry(const String& s) {
    ManagedWindowRep* w = rep();
    w->icon_geometry_ = new CopyString(s);
    w->do_set(this, &ManagedWindowRep::set_icon_geometry);
}

void ManagedWindow::icon_geometry(const char* s) {
    ManagedWindowRep* w = rep();
    w->icon_geometry_ = new CopyString(s);
    w->do_set(this, &ManagedWindowRep::set_icon_geometry);
}

void ManagedWindow::icon_bitmap(Bitmap* b) {
    ManagedWindowRep* w = rep();
    w->icon_bitmap_ = b;
    w->do_set(this, &ManagedWindowRep::set_icon_bitmap);
}

void ManagedWindow::icon_mask(Bitmap* b) {
    ManagedWindowRep* w = rep();
    w->icon_mask_ = b;
    w->do_set(this, &ManagedWindowRep::set_icon_mask);
}

void ManagedWindow::iconic(boolean b) {
    rep()->iconic_ = b;
}

void ManagedWindow::iconify() {
    WindowRep* w = Window::rep();
    XWindow xw = w->xwindow_;
    if (xw != WindowRep::unbound) {
	XEvent xe;
	static Atom a = None;
	DisplayRep* r = w->canvas_->rep()->display_->rep();
	XDisplay* dpy = r->display_;

	if (a == None) {
	    a = XInternAtom(dpy, "WM_CHANGE_STATE", False);
	}
	xe.type = ClientMessage;
	xe.xclient.type = ClientMessage;
	xe.xclient.display = dpy;
	xe.xclient.window = xw;
	xe.xclient.message_type = a;
	xe.xclient.format = 32;
	xe.xclient.data.l[0] = IconicState;
	XSendEvent(
	    dpy, r->root_, False,
	    SubstructureRedirectMask | SubstructureNotifyMask, &xe
	);
    }
}

void ManagedWindow::deiconify() {
    WindowRep* w = Window::rep();
    XWindow xw = w->xwindow_;
    if (xw != WindowRep::unbound) {
	XMapWindow(w->dpy(), xw);
    }
}

void ManagedWindow::resize() {
    default_geometry();
    ManagedWindowRep* w = rep();
    w->wm_normal_hints(this);
    Window::resize();
}

void ManagedWindow::focus_event(Handler* in, Handler* out) {
    WindowRep* w = Window::rep();
    Resource::ref(in);
    Resource::ref(out);
    Resource::unref(w->focus_in_);
    Resource::unref(w->focus_out_);
    w->focus_in_ = in;
    w->focus_out_ = out;
}

void ManagedWindow::wm_delete(Handler* h) {
    WindowRep* w = Window::rep();
    Resource::ref(h);
    Resource::unref(w->wm_delete_);
    w->wm_delete_ = h;
}

void ManagedWindow::configure() {
    Style* s = display()->style();
    String v;
    if (s->find_attribute(String("geometry"), v)) {
	geometry(v);
    }
    Window::configure();
}

void ManagedWindow::compute_geometry() {
    ManagedWindowRep* w = rep();
    WindowRep* wr = Window::rep();
    CanvasRep* c = wr->canvas_->rep();
    Display* d = wr->display_;
    unsigned int spec = 0;
    if (w->geometry_ != nil) {
	spec = XParseGeometry(
	    w->geometry_->string(),
	    &wr->xpos_, &wr->ypos_, &c->pwidth_, &c->pheight_
	);
	const unsigned int userplace = XValue | YValue;
	if ((spec & userplace) == userplace) {
	    wr->placed_ = true;
	}
	if ((spec & XValue) != 0 && (spec & XNegative) != 0) {
	    wr->xpos_ = d->pwidth() + wr->xpos_ - c->pwidth_;
	}
	if ((spec & YValue) != 0 && (spec & YNegative) != 0) {
	    wr->ypos_ = d->pheight() + wr->ypos_ - c->pheight_;
	}
    }
    if (c->pwidth_ <= 0) {
	c->width_ = 72;
	c->pwidth_ = d->to_pixels(c->width_);
    }
    if (c->pheight_ <= 0) {
	c->height_ = 72;
	c->pheight_ = d->to_pixels(c->height_);
    }
    if ((spec & WidthValue) != 0) {
	c->width_ = d->to_coord(c->pwidth_);
    }
    if ((spec & HeightValue) != 0) {
	c->height_ = d->to_coord(c->pheight_);
    }
}

void ManagedWindow::set_props() {
    ManagedWindowRep* w = rep();
    w->wm_normal_hints(this);
    w->wm_name(this);
    w->wm_class(this);
    w->wm_protocols(this);
    w->wm_colormap_windows(this);
    w->wm_hints(this);
}

/** class ApplicationWindow **/

ApplicationWindow::ApplicationWindow(Glyph* g) : ManagedWindow(g) { }
ApplicationWindow::~ApplicationWindow() { }

void ApplicationWindow::configure() {
    Style* s = display()->style();
    String v;
    if (s->find_attribute(String("title"), v)) {
	name(v);
    }
    if (s->find_attribute(String("iconGeometry"), v)) {
	icon_geometry(v);
    }
    if (s->value_is_on(String("iconic"))) {
	iconic(true);
    }
    ManagedWindow::configure();
}

void ApplicationWindow::set_props() {
    ManagedWindowRep* w = rep();
    Session* s = Session::instance();
    XSetCommand(
	s->default_display()->rep()->display_, Window::rep()->xwindow_,
	s->argv(), s->argc()
    );
    if (w->icon_name_ == nil) {
	w->icon_name_ = new CopyString(*w->name_);
    }
    ManagedWindow::set_props();
}

/** class TopLevelWindow **/

TopLevelWindow::TopLevelWindow(Glyph* g) : ManagedWindow(g) { }
TopLevelWindow::~TopLevelWindow() { }

void TopLevelWindow::group_leader(Window* primary) {
    ManagedWindowRep* w = rep();
    w->group_leader_ = primary;
    w->do_set(this, &ManagedWindowRep::set_group_leader);
}

Window* TopLevelWindow::group_leader() const { return rep()->group_leader_; }

void TopLevelWindow::set_props() {
    ManagedWindowRep* w = rep();
    if (w->icon_name_ == nil) {
	w->icon_name_ = new CopyString(*w->name_);
    }
    ManagedWindow::set_props();
}

/** class TransientWindow **/

TransientWindow::TransientWindow(Glyph* g) : TopLevelWindow(g) { }
TransientWindow::~TransientWindow() { }

void TransientWindow::transient_for(Window* primary) {
    ManagedWindowRep* w = rep();
    w->transient_for_ = primary;
    w->do_set(this, &ManagedWindowRep::set_transient_for);
}

Window* TransientWindow::transient_for() const {
    return rep()->transient_for_;
}

/*
 * Don't do the normal geometry property lookup, etc. for transients.
 */

void TransientWindow::configure() {
    Window::configure();
}

void TransientWindow::set_attributes() {
    TopLevelWindow::set_attributes();
    save_under(true);
}

/** class PopupWindow **/

PopupWindow::PopupWindow(Glyph* g) : Window(g) { }
PopupWindow::~PopupWindow() { }

void PopupWindow::set_attributes() {
    Window::set_attributes();
    WindowRep* w = rep();
    save_under(true);
    w->xattrmask_ |= CWOverrideRedirect;
    w->xattrs_.override_redirect = True;
}

/** class IconWindow **/

IconWindow::IconWindow(Glyph* g) : ManagedWindow(g) { }
IconWindow::~IconWindow() { }

void IconWindow::do_map() { }

/** class WindowRep **/

/*
 * Hopefully, these atoms will have the same values on all displays.
 */

XDisplay* WindowRep::dpy() { return display_->rep()->display_; }

Atom WindowRep::wm_delete_atom_ = None;

Atom WindowRep::wm_delete_atom() {
    if (wm_delete_atom_ == None) {
	wm_delete_atom_ = XInternAtom(dpy(), "WM_DELETE_WINDOW", False);
    }
    return wm_delete_atom_;
}

void WindowRep::clear_mapping_info() {
    xtoplevel_ = WindowRep::unbound;
    needs_resize_ = false;
    resized_ = false;
    placed_ = false;
    aligned_ = false;
    moved_ = false;
}

void WindowRep::map_notify(Window*, XMapEvent&) {
    needs_resize_ = true;
    canvas_->rep()->status_ = Canvas::mapped;
}

/*
 * We can only see an unmap if it is generated external (e.g.,
 * by a window manager).  Application unmaps will unbind the window,
 * thus removing it from the xid->window table.
 */

void WindowRep::unmap_notify(Window*, XUnmapEvent&) {
    canvas_->rep()->status_ = Canvas::unmapped;
}

/*
 * Handle an expose event.  Because window managers generate a variety
 * of event sequences in response to maps, We defer the first resize
 * until when a window is first exposed.
 */

void WindowRep::expose(Window* w, XExposeEvent& xe) {
    unsigned int pw = canvas_->pwidth();
    unsigned int ph = canvas_->pheight();
    if (needs_resize_) {
	needs_resize_ = false;
	resize(w, pw, ph);
    } else {
	Display* d = display_;
	Coord l = d->to_coord(xe.x);
	Coord r = l + d->to_coord(xe.width);
	Coord t = d->to_coord(ph - xe.y);
	Coord b = t - d->to_coord(xe.height);
	canvas_->redraw(l, b, r, t);
    }
}

/*
 * Handle an X ConfigureNotify event.  If the window has been configured
 * once already, then only resize it if the new size is different.
 * If it hasn't been configured once, then note the size and we'll take
 * care of it when the first expose event is handled.
 */

void WindowRep::configure_notify(Window* w, XConfigureEvent& xe) {
    moved_ = true;
    if (resized_) {
	if (xe.width != canvas_->pwidth() || xe.height != canvas_->pheight()) {
	    resize(w, xe.width, xe.height);
	}
    } else {
	canvas_->psize(xe.width, xe.height);
	needs_resize_ = true;
    }
}

/*
 * Note that a window has moved.
 */

void WindowRep::move(Window*, int x, int y) {
    xpos_ = x;
    ypos_ = y;
    moved_ = false;
}

/*
 * Resize a window, allocating the associated glyph and
 * damaging the new area.
 */

boolean WindowRep::resize(
    Window* w, unsigned int xwidth, unsigned int xheight
) {
    resized_ = true;
    canvas_->psize(xwidth, xheight);
    canvas_->damage_all();
    Coord xsize = canvas_->width();
    float xalign = shape_.requirement(Dimension_X).alignment();
    Coord ysize = canvas_->height();
    float yalign = shape_.requirement(Dimension_Y).alignment();
    Allotment ax(xalign * xsize, xsize, xalign);
    Allotment ay(yalign * ysize, ysize, yalign);
    allocation_.allot(Dimension_X, ax);
    allocation_.allot(Dimension_Y, ay);
    Extension ext;
    ext.xy_extents(fil, -fil, fil, -fil);
    glyph_->allocate(canvas_, allocation_, ext);
    init_renderer(w);
    return true;
}

void WindowRep::check_position(const Window*) {
    if (moved_) {
	DisplayRep* d = display_->rep();
	int x, y;
	XWindow xw;
	XTranslateCoordinates(
	    d->display_, xwindow_, d->root_, 0, 0, &x, &y, &xw
	);
	xpos_ = x;
	ypos_ = display_->pheight() - y - canvas_->pheight();
	moved_ = false;
    }
}

void WindowRep::do_bind(Window* w, XWindow parent, int left, int top) {
    CanvasRep* c = canvas_->rep();
    DisplayRep* d = display_->rep();
    XDisplay* dpy = d->display_;
    WindowTable* t = d->wtable_;
    if (xwindow_ != WindowRep::unbound) {
	t->remove(xwindow_);
    }
    w->set_attributes();
    xwindow_ = XCreateWindow(
	dpy, parent, left, top, canvas_->pwidth(), canvas_->pheight(),
	/* border width */ 0, d->depth_, xclass_,
	d->visual_, xattrmask_, &xattrs_
    );
    c->xdrawable_ = xwindow_;
    t->insert(xwindow_, w);
    xtoplevel_ = toplevel_->rep()->xwindow_;
}

void WindowRep::init_renderer(Window* w) {
    CanvasRep* c = w->canvas()->rep();
    c->unbind();
    c->bind(double_buffered_);
}

Window* WindowRep::find(XWindow xw, WindowTable* t) {
    Window* window;
    if (t->find(window, xw)) {
	WindowRep* w = window->rep();
	if (w->toplevel_->rep()->xwindow_ == w->xtoplevel_) {
	    return window;
	}
    }
    return nil;
}

/* class ManagedWindowRep */

void ManagedWindowRep::do_set(Window* window, HintFunction f) {
    WindowRep* w = window->rep();
    ManagedWindowHintInfo info;
    info.xwindow_ = w->xwindow_;
    if (info.xwindow_ != WindowRep::unbound) {
	info.dpy_ = w->dpy();
	info.hints_ = XGetWMHints(info.dpy_, info.xwindow_);
	if (info.hints_ == nil) {
	    info.hints_ = XAllocWMHints();
	}
	info.pwidth_ = w->canvas_->pwidth();
	info.pheight_ = w->canvas_->pheight();
	info.display_ = w->display_;
	if ((this->*f)(info)) {
	    XSetWMHints(info.dpy_, info.xwindow_, info.hints_);
	}
	XFree((char*)info.hints_);
    }
}

boolean ManagedWindowRep::set_name(ManagedWindowHintInfo& info) {
    if (name_ != nil) {
	XStoreName(info.dpy_, info.xwindow_, name_->string());
    }
    return false;
}

boolean ManagedWindowRep::set_geometry(ManagedWindowHintInfo&) {
    /* unimplemented: should configure mapped windows */
    return false;
}

boolean ManagedWindowRep::set_group_leader(ManagedWindowHintInfo& info) {
    if (group_leader_ == nil) {
	info.hints_->flags &= ~WindowGroupHint;
	info.hints_->window_group = None;
	return true;
    } else {
	XWindow g = group_leader_->rep()->xwindow_;
	if (g != WindowRep::unbound) {
	    info.hints_->flags |= WindowGroupHint;
	    info.hints_->window_group = g;
	    return true;
	}
    }
    return false;
}

boolean ManagedWindowRep::set_transient_for(ManagedWindowHintInfo& info) {
    if (transient_for_ != nil) {
	XDrawable td = transient_for_->rep()->xwindow_;
	if (td != WindowRep::unbound) {
	    XSetTransientForHint(info.dpy_, info.xwindow_, td);
	    return true;
	}
    }
    return false;
}

boolean ManagedWindowRep::set_icon(ManagedWindowHintInfo& info) {
    if (icon_ == nil) {
	info.hints_->flags &= ~IconWindowHint;
	info.hints_->icon_window = None;
	return true;
    } else {
	XWindow i = icon_->Window::rep()->xwindow_;
	if (i != WindowRep::unbound) {
	    info.hints_->flags |= IconWindowHint;
	    info.hints_->icon_window = i;
	    return true;
	}
    }
    return false;
}

boolean ManagedWindowRep::set_icon_name(ManagedWindowHintInfo& info) {
    if (icon_name_ != nil) {
	XSetIconName(info.dpy_, info.xwindow_, icon_name_->string());
    }
    return false;
}

boolean ManagedWindowRep::set_icon_geometry(ManagedWindowHintInfo& info) {
    info.hints_->flags &= ~IconPositionHint;
    const String* g = icon_geometry_;
    if (g == nil && icon_ != nil) {
	g = icon_->geometry();
    }
    if (g != nil) {
	int x = 0, y = 0;
	unsigned int w = info.pwidth_;
	unsigned int h = info.pheight_;
	if (icon_bitmap_ != nil) {
	    w = icon_bitmap_->pwidth();
	    h = icon_bitmap_->pheight();
	}
	if (icon_ != nil) {
	    w = icon_->pwidth();
	    h = icon_->pheight();
	}
	unsigned int p = XParseGeometry(g->string(), &x, &y, &w, &h);
	Display* d = info.display_;
	if ((p & XNegative) != 0) {
	    x = d->pwidth() + x - w;
	}
	if ((p & YNegative) != 0) {
	    y = d->pheight() + y - h;
	}
	if ((p & (XValue|YValue)) != 0) {
	    info.hints_->flags |= IconPositionHint;
	    info.hints_->icon_x = x;
	    info.hints_->icon_y = y;
	    return true;
	}
    }
    return false;
}

boolean ManagedWindowRep::set_icon_bitmap(ManagedWindowHintInfo& info) {
    if (icon_bitmap_ == nil) {
	info.hints_->flags &= ~IconPixmapHint;
	info.hints_->icon_pixmap = None;
    } else {
	info.hints_->flags |= IconPixmapHint;
	info.hints_->icon_pixmap = icon_bitmap_->rep()->pixmap_;
    }
    return true;
}

boolean ManagedWindowRep::set_icon_mask(ManagedWindowHintInfo& info) {
    if (icon_mask_ == nil) {
	info.hints_->flags &= ~IconMaskHint;
	info.hints_->icon_mask = None;
    } else {
	info.hints_->flags |= IconMaskHint;
	info.hints_->icon_mask = icon_mask_->rep()->pixmap_;
    }
    return true;
}

boolean ManagedWindowRep::set_all(ManagedWindowHintInfo& info) {
    info.hints_->flags = InputHint;
    info.hints_->input = True;
    info.hints_->flags |= StateHint;
    info.hints_->initial_state = iconic_ ? IconicState : NormalState;
    set_name(info);
    set_geometry(info);
    set_group_leader(info);
    set_transient_for(info);
    set_icon_name(info);
    set_icon_geometry(info);
    set_icon(info);
    set_icon_bitmap(info);
    set_icon_mask(info);
    return true;
}

void ManagedWindowRep::wm_normal_hints(Window* window) {
    WindowRep* w = window->rep();
    Display* d = w->display_;
    Coord cwidth = w->canvas_->width();
    Coord cheight = w->canvas_->height();
    unsigned int cpwidth = w->canvas_->pwidth();
    unsigned int cpheight = w->canvas_->pheight();
    XSizeHints sizehints;
    if (w->placed_) {
	sizehints.flags = USPosition | USSize;
    } else {
	sizehints.flags = PSize | PBaseSize;
    }
    /* obsolete as of R4, but kept for backward compatibility */
    sizehints.x = w->xpos_;
    sizehints.y = w->ypos_;
    sizehints.width = cpwidth;
    sizehints.height = cpheight;

    sizehints.base_width = cpwidth;
    sizehints.base_height = cpheight;

    const Coord smallest = d->to_coord(2);
    const Coord x_largest = d->width();
    const Coord y_largest = d->height();
    Requirement& rx = w->shape_.requirement(Dimension_X);
    Requirement& ry = w->shape_.requirement(Dimension_Y);
    Coord min_width = Math::min(
	x_largest, Math::max(smallest, cwidth - rx.shrink())
    );
    Coord min_height = Math::min(
	y_largest, Math::max(smallest, cheight - ry.shrink())
    );
    Coord max_width = Math::max(
	smallest, Math::min(x_largest, cwidth + rx.stretch())
    );
    Coord max_height = Math::max(
	smallest, Math::min(y_largest, cheight + ry.stretch())
    );

    sizehints.flags |= PMinSize;
    sizehints.min_width = d->to_pixels(min_width);
    sizehints.min_height = d->to_pixels(min_height);
    sizehints.flags |= PMaxSize;
    sizehints.max_width = d->to_pixels(max_width);
    sizehints.max_height = d->to_pixels(max_height);

    /* PResizeInc: width_inc, height_inc */
    /* PAspect: {min_aspect,max_aspect}.{x,y} */

    XSetNormalHints(w->dpy(), w->xwindow_, &sizehints);
}

void ManagedWindowRep::wm_name(Window* window) {
    WindowRep* w = window->rep();
    if (name_ == nil) {
	name_ = new CopyString(Session::instance()->name());
    }
    String hostname(Host::name());
    XChangeProperty(
	w->dpy(), w->xwindow_, XA_WM_CLIENT_MACHINE,
	XA_STRING, 8, PropModeReplace,
	(unsigned char*)(hostname.string()), hostname.length()
    );
}

void ManagedWindowRep::wm_class(Window* window) {
    WindowRep* w = window->rep();
    XClassHint classhint;
    classhint.res_name = (char*)(name_->string());
    classhint.res_class = (char*)(Session::instance()->classname());
    XSetClassHint(w->dpy(), w->xwindow_, &classhint);
}

void ManagedWindowRep::wm_protocols(Window* window) {
    WindowRep* w = window->rep();
    Atom a = w->wm_delete_atom();
    XSetWMProtocols(w->dpy(), w->xwindow_, &a, 1);
}

void ManagedWindowRep::wm_colormap_windows(Window*) {
    /* we do not currently manipulate colormaps */
}

void ManagedWindowRep::wm_hints(Window* window) {
    do_set(window, &ManagedWindowRep::set_all);
}
