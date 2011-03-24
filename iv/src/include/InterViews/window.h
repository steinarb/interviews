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
 * Window - top-level object for interfacing with window managers
 */

#ifndef iv_window_h
#define iv_window_h

#include <InterViews/canvas.h>
#include <InterViews/coord.h>

#include <InterViews/_enter.h>

class Bitmap;
class Canvas;
class Cursor;
class Display;
class Event;
class Glyph;
class Handler;
class String;

class Window {
protected:
    Window(Glyph*);
public:
    virtual ~Window();

    virtual void display(Display*);
    virtual Display* display() const;

    virtual Canvas* canvas() const;

    virtual void save_under(boolean);
    virtual boolean save_under() const;

    virtual void save_contents(boolean);
    virtual boolean save_contents() const;

    virtual void double_buffered(boolean);
    virtual boolean double_buffered() const;

    virtual void cursor(Cursor*);
    virtual Cursor* cursor() const;

    virtual void place(Coord left, Coord bottom);
    virtual void pplace(IntCoord left, IntCoord bottom);
    virtual void align(float x, float y);
    virtual Coord left() const;
    virtual Coord bottom() const;

    virtual Coord width() const;
    virtual Coord height() const;
    virtual unsigned int pwidth() const;
    virtual unsigned int pheight() const;

    virtual void map();
    virtual void unmap();

    virtual void bind();
    virtual void unbind();
    virtual boolean bound() const;

    virtual void raise();
    virtual void lower();
    virtual void move(Coord left, Coord bottom);
    virtual void resize();

    virtual void receive(const Event&);
    virtual Handler* target(const Event&) const;
    virtual void grab_pointer(Cursor* = nil) const;
    virtual void ungrab_pointer() const;

    virtual void repair();

    class WindowRep* rep() const;
protected:
    Window(WindowRep*);

    virtual void configure();
    virtual void default_geometry();
    virtual void compute_geometry();
    virtual void set_attributes();
    virtual void set_props();
    virtual void do_map();
private:
    friend class WindowRep;

    WindowRep* rep_;
};

inline WindowRep* Window::rep() const { return rep_; }

class ManagedWindow : public Window {
protected:
    ManagedWindow(Glyph*);
public:
    virtual ~ManagedWindow();

    virtual void display(Display*);
    virtual Display* display() const;

    virtual void geometry(const String&);
    virtual void geometry(const char*);
    virtual const String* geometry() const;

    virtual void name(const String&);
    virtual void name(const char*);
    virtual const String* name() const;

    virtual void icon_name(const String&);
    virtual void icon_name(const char*);
    virtual const String* icon_name() const;

    virtual void icon(ManagedWindow*);
    virtual ManagedWindow* icon() const;

    virtual void icon_geometry(const String&);
    virtual void icon_geometry(const char*);
    virtual const String* icon_geometry() const;

    virtual void icon_bitmap(Bitmap*);
    virtual Bitmap* icon_bitmap() const;

    virtual void icon_mask(Bitmap*);
    virtual Bitmap* icon_mask() const;

    virtual void iconic(boolean);
    virtual boolean iconic() const;

    virtual void iconify();
    virtual void deiconify();

    virtual void focus_event(Handler* in, Handler* out);
    virtual void wm_delete(Handler*);

    virtual void resize();

    class ManagedWindowRep* rep() const;
protected:
    virtual void configure();
    virtual void compute_geometry();
    virtual void set_props();
private:
    ManagedWindowRep* rep_;
};

inline ManagedWindowRep* ManagedWindow::rep() const { return rep_; }

class ApplicationWindow : public ManagedWindow {
public:
    ApplicationWindow(Glyph*);
    ~ApplicationWindow();
protected:
    virtual void configure();
    virtual void set_props();
};

class TopLevelWindow : public ManagedWindow {
public:
    TopLevelWindow(Glyph*);
    ~TopLevelWindow();

    virtual void group_leader(Window*);
    virtual Window* group_leader() const;
protected:
    virtual void set_props();
};

class TransientWindow : public TopLevelWindow {
public:
    TransientWindow(Glyph*);
    ~TransientWindow();

    virtual void transient_for(Window*);
    virtual Window* transient_for() const;
protected:
    virtual void configure();
    virtual void set_attributes();
};

class PopupWindow : public Window {
public:
    PopupWindow(Glyph*);
    ~PopupWindow();
protected:
    virtual void set_attributes();
};

class IconWindow : public ManagedWindow {
public:
    IconWindow(Glyph*);
    ~IconWindow();
protected:
    virtual void do_map();
};

#include <InterViews/_leave.h>

#endif
