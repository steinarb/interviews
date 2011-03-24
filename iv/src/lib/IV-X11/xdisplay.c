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
 * X11-dependent display code
 */

#include "damagelist.h"
#include "wtable.h"
#include <InterViews/color.h>
#include <InterViews/display.h>
#include <InterViews/event.h>
#include <InterViews/handler.h>
#include <InterViews/style.h>
#include <InterViews/window.h>
#include <IV-X11/Xlib.h>
#include <IV-X11/Xutil.h>
#include <IV-X11/Xext.h>
#include <IV-X11/xdisplay.h>
#include <IV-X11/xevent.h>
#include <IV-X11/xwindow.h>
#include <X11/Xatom.h>
#include <OS/list.h>
#include <OS/math.h>
#include <OS/string.h>
#include <osfcn.h>
#include <stdio.h>
#include <sys/ioctl.h>

declareList(GrabList,Handler*);
implementList(GrabList,Handler*);

implementList(DamageList,Window*);

implementTable(WindowTable,XWindow,Window*);

declareTable(ColorTable,unsigned long,XColor);
implementTable(ColorTable,unsigned long,XColor);

class RGBTableEntry {
public:
    unsigned short red_;
    unsigned short green_;
    unsigned short blue_;

    unsigned long hash() const;
    boolean operator ==(const RGBTableEntry&) const;
    boolean operator !=(const RGBTableEntry&) const;
};

inline unsigned long key_to_hash(const RGBTableEntry& k) { return k.hash(); }

unsigned long RGBTableEntry::hash() const {
    return (red_ >> 7) ^ (green_ >> 7) ^ (blue_ >> 7);
}

boolean RGBTableEntry::operator ==(const RGBTableEntry& rgb) const {
    return red_ == rgb.red_ && green_ == rgb.green_ && blue_ == rgb.blue_;
}

boolean RGBTableEntry::operator !=(const RGBTableEntry& rgb) const {
    return red_ != rgb.red_ || green_ != rgb.green_ || blue_ != rgb.blue_;
}

declareTable(RGBTable,RGBTableEntry,XColor);
implementTable(RGBTable,RGBTableEntry,XColor);

Display::Display(DisplayRep* d) {
    rep_ = d;
}

Display* Display::open(const String& s) {
    NullTerminatedString ns(s);
    return open(ns.string());
}

Display* Display::open() {
    return open(nil);
}

Display* Display::open(const char* device) {
    XDisplay* dpy = XOpenDisplay(device);
    if (dpy == nil) {
	return nil;
    }
    DisplayRep* d = new DisplayRep;
    d->display_ = dpy;
    d->screen_ = DefaultScreen(d->display_);
    d->style_ = nil;
    d->grabbers_ = new GrabList;
    d->damage_list_ = new DamageList;
    d->wtable_ = new WindowTable(1024);
    return new Display(d);
}

void Display::close() {
    DisplayRep* d = rep();
    XCloseDisplay(d->display_);
}

Display::~Display() {
    DisplayRep* d = rep();
    Resource::unref(d->style_);
    delete d->ctable_;
    delete d->rgbtable_;
#ifdef __GNUC__
    delete [d->localmapsize_] d->localmap_;
#else
    delete [] d->localmap_;
#endif
    delete d->grabbers_;
    delete d->damage_list_;
    delete d->wtable_;
    delete d;
}

int Display::fd() const { return ConnectionNumber(rep()->display_); }
Coord Display::width() const { return rep()->width_; }
Coord Display::height() const { return rep()->height_; }
unsigned int Display::pwidth() const { return rep()->pwidth_; }
unsigned int Display::pheight() const { return rep()->pheight_; }

/*
 * Convert millimeters to points.  We use 72.27 pts/in and 25.4 mm/in.
 */

static inline double mm_to_points(double mm) {
    return (72.27 / 25.4) * mm;
}

Coord Display::a_width() const {
    DisplayRep& d = *rep();
    return Coord(mm_to_points(double(DisplayWidthMM(d.display_, d.screen_))));
}

Coord Display::a_height() const {
    DisplayRep& d = *rep();
    return Coord(mm_to_points(double(DisplayHeightMM(d.display_, d.screen_))));
}

boolean Display::defaults(String& s) const {
    const char* list = rep_->display_->xdefaults;
    if (list != nil) {
	s = list;
	return true;
    }
    return false;
}

static void fixup(
    Style* s, const String& name, const String& value, const char* msg
) {
    String v;
    if (s->find_attribute(name, v)) {
	fprintf(
	    stderr, "Warning: %s \"%.*s\" is not defined (using \"%.*s\") \n",
	    msg, v.length(), v.string(), value.length(), value.string()
	);
    } else {
	fprintf(
	    stderr, "Warning: No default %.*s attribute (using \"%.*s\") \n",
	    name.length(), name.string(), value.length(), value.string()
	);
    }
    s->attribute(name, value);
}

void Display::style(Style* s) {
    DisplayRep& d = *rep();
    Resource::ref(s);
    Resource::unref(d.style_);
    d.style_ = s;
    set_screen(d.screen_);
    d.set_shaped_windows();
    d.set_synchronous();
    d.set_xor_pixel();
    if (s->font() == nil) {
	fixup(s, String("font"), String("fixed"), "Font");
    }
    if (s->foreground() == nil) {
	fixup(s, String("foreground"), String("#000000"), "Color");
    }
    if (s->background() == nil) {
	fixup(s, String("background"), String("#ffffff"), "Color");
    }
    if (s->flat() == nil) {
	fixup(s, String("flat"), String("#c0c0c0"), "Color");
    }
}
    
Style* Display::style() const { return rep()->style_; }

void Display::set_screen(int s) {
    DisplayRep& d = *rep();
    XDisplay* dpy = d.display_;
    if (s < 0 || s >= ScreenCount(dpy)) {
	return;
    }
    d.screen_ = s;
    d.root_ = RootWindow(dpy, s);
    d.depth_ = DefaultDepth(dpy, s);
    d.set_visual();
    d.pwidth_ = DisplayWidth(dpy, s);
    d.pheight_ = DisplayHeight(dpy, s);
    d.set_dpi(pixel_);
    d.width_ = to_coord(d.pwidth_);
    d.height_ = to_coord(d.pheight_);
}

void Display::repair() {
    DamageList& list = *rep()->damage_list_;
    for (ListItr(DamageList) i(list); i.more(); i.next()) {
	i.cur()->repair();
    }
    list.remove_all();
}

void Display::flush() {
    repair();
    XFlush(rep()->display_);
}

void Display::sync() {
    repair();
    XSync(rep()->display_, 0);
}

void Display::ring_bell(int v) {
    XDisplay* dpy = rep()->display_;
    if (v > 100) {
	XBell(dpy, 100);
    } else if (v >= 0) {
	XBell(dpy, v);
    }
}

void Display::set_key_click(int v) {
    XKeyboardControl k;
    k.key_click_percent = v;
    XChangeKeyboardControl(rep()->display_, KBKeyClickPercent, &k);
}

void Display::set_auto_repeat(boolean b) {
    XDisplay* dpy = rep()->display_;
    if (b) {
	XAutoRepeatOn(dpy);
    } else {
	XAutoRepeatOff(dpy);
    }
}

void Display::set_pointer_feedback(int t, int s) {
    XChangePointerControl(rep()->display_, True, True, s, 1, t);
}

void Display::move_pointer(Coord x, Coord y) {
    DisplayRep& d = *rep();
    XWarpPointer(
	d.display_, None, d.root_, 0, 0, 0, 0,
	to_pixels(x), pheight() - to_pixels(y)
    );
}

/*
 * Set the visual to use for the display.  Start with the default one, and
 * use the "visual_id" or "visual" attributes to override.
 */

void DisplayRep::set_visual() {
    visual_ = DefaultVisual(display_, screen_);
    cmap_ = DefaultColormap(display_, screen_);
    String s;
    if (style_->find_attribute("visual_id", s)) {
	long id;
	if (s.convert(id)) {
	    XVisualInfo info;
	    info.visualid = id;
	    set_visual_by_info(info, VisualIDMask);
	}
    } else if (style_->find_attribute("visual", s)) {
	set_visual_by_class_name(s);
    }
    set_color_tables();
}

/*
 * Lookup a visual by class name.
 */

struct VisualTable {
    char* class_name;
    int class_tag;
};

static VisualTable visual_classes[] = {
    { "TrueColor", TrueColor },
    { "PseudoColor", PseudoColor },
    { "StaticGray", StaticGray },
    { "GrayScale", GrayScale },
    { "StaticColor", StaticColor },
    { "DirectColor", DirectColor },
    { nil, -1 }
};

void DisplayRep::set_visual_by_class_name(const String& name) {
    for (VisualTable* v = &visual_classes[0]; v->class_name != nil; v++) {
	if (name == v->class_name) {
	    XVisualInfo info;
	    info.c_class = v->class_tag;
	    set_visual_by_info(info, VisualClassMask);
	    return;
	}
    }
}

void DisplayRep::set_visual_by_info(XVisualInfo& info, long mask) {
    info.screen = screen_;
    int nvisuals = 0;
    XVisualInfo* visuals = XGetVisualInfo(
	display_, VisualScreenMask | mask, &info, &nvisuals
    );
    if (visuals != nil) {
	if (nvisuals > 0) {
	    visual_ = visuals[0].visual;
	    depth_ = visuals[0].depth;
	    cmap_ = XCreateColormap(display_, root_, visual_, AllocNone);
	}
	XFree((char*)visuals);
    }
}

/*
 * Set up appropriate color mapping tables for the visual.
 * For TrueColor, we don't need an rgb->pixel table because we can
 * compute the pixel value directly.  The pixel->rgb table (ctable_)
 * is used by Raster::peek.
 *
 * The table sizes are 512 = 2 (hash tables work best half full) times
 * 256 (most non-TrueColor systems are 8-bit).
 */

void DisplayRep::set_color_tables() {
    switch (visual_->c_class) {
    case TrueColor:
	rgbtable_ = nil;
	set_shift(visual_->red_mask, red_, red_shift_);
	set_shift(visual_->green_mask, green_, green_shift_);
	set_shift(visual_->blue_mask, blue_, blue_shift_);
	break;
    default:
	rgbtable_ = new RGBTable(512);
	break;
    }
    ctable_ = new ColorTable(512);
    localmap_ = nil;
    localmapsize_ = 0;
}

void DisplayRep::set_shift(
    unsigned long mask, unsigned long& v, unsigned long& shift
) {
    shift = 0;
    v = mask;
    while ((v & 0x1) == 0) {
	shift += 1;
	v >>= 1;
    }
}

void DisplayRep::set_shaped_windows() {
    if (style_->value_is_on("shaped_windows")) {
	int d1, d2;
	if (!XShapeQueryExtension(display_, &d1, &d2)) {
	    style_->attribute("shaped_windows", "off");
	}
    }
}

void DisplayRep::set_synchronous() {
    if (style_->value_is_on("synchronous")) {
	XSynchronize(display_, True);
    }
}

/*
 * Compute a reasonable pixel for xor'ing.  Note that this should be done
 * after the visual is selected to handle the DirectColor case correctly.
 */

static inline unsigned int MSB(unsigned long i) {
    return (i ^ (i>>1)) & i;
}

void DisplayRep::set_xor_pixel() {
    String custom;
    if (style_->find_attribute(String("RubberbandPixel"), custom)) {
	long n = 1;
	custom.convert(n);
	xor_ = n;
    } else if (visual_->c_class == DirectColor) {
        xor_ = (
            MSB(visual_->red_mask) |
	    MSB(visual_->green_mask) |
            MSB(visual_->blue_mask)
        );
    } else {
	xor_ = BlackPixel(display_, screen_) ^ WhitePixel(display_, screen_);
    }
}

/*
 * Compute size of a pixel in printer points.  If the "dpi" attribute
 * is specified, then we use it as dots per inch and convert to points.
 * Otherwise we use font metrics, not the (alleged) screen size,
 * because applications really care about how things measure
 * with respect to text.  The default assumes that fonts are designed
 * for 75 dots/inch and printer points are 72 pts/inch.
 */

void DisplayRep::set_dpi(Coord& pixel) {
    String s;
    if (style_->find_attribute("dpi", s)) {
	long dpi;
	if (s.convert(dpi) && dpi != 0) {
	    pixel = 72.0 / float(dpi);
	}
    } else {
	pixel = 72.0 / 75.0;
    }
}

/*
 * Find the X color information for a given pixel value.
 * If it is already in the color table, just retrieve.
 * Otherwise, we have to query X.
 */

void DisplayRep::find_color(unsigned long pixel, XColor& xc) {
    if (!ctable_->find(xc, pixel)) {
	xc.pixel = pixel;
	XQueryColor(display_, cmap_, &xc);
	ctable_->insert(pixel, xc);
    }
}

static double distance(
    unsigned short r, unsigned short g, unsigned short b, const XColor& xc
) {
    double rr = r - xc.red;
    double gg = g - xc.green;
    double bb = b - xc.blue;
    return rr*rr + gg*gg + bb*bb;
}

static inline unsigned long rescale(
    unsigned long value, unsigned long in_scale, unsigned long out_scale
) {
    return (value * out_scale + in_scale/2) / in_scale;
}

/*
 * Find the X color information for a specified rgb.
 * For a TrueColor visual, this is easy (computed directly from rgb values).
 * Otherwise, we have to do an XAllocColor if we haven't seen the rgb
 * combination before.  If XAllocColor fails, then we read the colormap and
 * try to find the best match.  Note this may cause havoc if the colormap
 * entries are read/write.
 */

void DisplayRep::find_color(
    unsigned short red, unsigned short green, unsigned short blue, XColor& xc
) {
    switch (visual_->c_class) {
    case TrueColor:
	unsigned long r = rescale(red, 0xffff, red_);
	unsigned long g = rescale(green, 0xffff, green_);
	unsigned long b = rescale(blue, 0xffff, blue_);
	xc.pixel = (
	    (r << red_shift_) | (g << green_shift_) | (b << blue_shift_)
	);
	xc.red = (unsigned short)rescale(r, red_, 0xffff);
	xc.green = (unsigned short)rescale(g, green_, 0xffff);
	xc.blue = (unsigned short)rescale(b, blue_, 0xffff);
	break;
    default:
	RGBTableEntry rgb;
	rgb.red_ = red;
	rgb.green_ = green;
	rgb.blue_ = blue;
	if (!rgbtable_->find(xc, rgb)) {
	    if (localmapsize_ == 0) {
		xc.red = red;
		xc.green = green;
		xc.blue = blue;
		if (!XAllocColor(display_, cmap_, &xc)) {
		    localmapsize_ = Math::min(visual_->map_entries, 256);
		    localmap_ = new XColor[localmapsize_];
		    for (unsigned long p = 0; p < localmapsize_; p++) {
			localmap_[p].pixel = p;
		    }
		    XQueryColors(display_, cmap_, localmap_, localmapsize_);
		}
	    }
	    if (localmapsize_ != 0) {
		unsigned long best = 0;
		double best_match = distance(red, green, blue, localmap_[0]);
		for (unsigned long p = 1; p < localmapsize_; p++) {
		    double match = distance(red, green, blue, localmap_[p]);
		    if (match < best_match) {
			best = p;
			best_match = match;
		    }
		}
		xc = localmap_[best];
	    }
	    rgbtable_->insert(rgb, xc);
	}
    }
}

/*
 * Read the next event if one is pending.  Otherwise, return false.
 * Window::receive will be called on the target window for the event,
 * if the window is known and is valid.  Because we don't keep track
 * of subwindows, it is possible to get an event for a subwindow after
 * the main window has been unmapped.  We must ignore such events.
 */

boolean Display::get(Event& event) {
    DisplayRep* d = rep();
    EventRep& e = *(event.rep());
    e.display_ = this;
    XDisplay* dpy = d->display_;
    XEvent& xe = e.xevent_;
    if (d->damage_list_->count() != 0 && QLength(dpy) == 0) {
	repair();
    }
    if (!XPending(dpy)) {
	return false;
    }
    XNextEvent(dpy, &xe);
    e.clear();
    e.window_ = WindowRep::find(xe.xany.window, d->wtable_);
    if (e.window_ != nil) {
	e.window_->receive(event);
    }
    return true;
}

void Display::put(const Event& e) {
    XPutBackEvent(rep()->display_, &e.rep()->xevent_);
}

/*
 * Check to see if the display connection just shut down.
 */

boolean Display::closed() {
    XDisplay* dpy = rep()->display_;
    if (XEventsQueued(dpy, QueuedAfterReading) == 0) {
	/* need to detect whether partial event or connection closed */
	int fd = ConnectionNumber(dpy);
	int pending = 0;
	if (ioctl(fd, FIONREAD, (char*)&pending) < 0 || pending == 0) {
	    return true;
	}
    }
    return false;
}

/*
 * Add a handler to the grabber list.  The handler is ref'd to ensure
 * that is not deallocated while on the list.
 */

void Display::grab(Handler* h) {
    GrabList& g = *(rep()->grabbers_);
    h->ref();
    g.append(h);
}

/*
 * Remove a handler from the grabber list.
 * This function has no effect if the handler is not presently on the list.
 * If the handler is on the list, it is unref'd.
 */

void Display::ungrab(Handler* h) {
    for (ListItr(GrabList) i(*rep()->grabbers_); i.more(); i.next()) {
	if (i.cur() == h) {
	    i.remove_cur();
	    h->unref();
	    break;
	}
    }
}

/*
 * Return the most recent grabber, or nil if the list is empty.
 */

Handler* Display::grabber() const {
    GrabList& g = *rep()->grabbers_;
    long n = g.count();
    return (n == 0) ? nil : g.item(n - 1);
}

/*
 * Check whether a given handler is on the grabber list.
 */

boolean Display::is_grabbing(Handler* h) const {
    for (ListItr(GrabList) i(*rep()->grabbers_); i.more(); i.next()) {
	if (i.cur() == h) {
	    return true;
	}
    }
    return false;
}
