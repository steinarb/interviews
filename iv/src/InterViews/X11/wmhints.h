/*
 * Copyright (c) 1989 Stanford University
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
 * Interface to X11 window manager and related hints
 */

#ifndef X11_wmhints_h
#define X11_wmhints_h

#include <InterViews/X11/Xlib.h>
#include <InterViews/X11/Xutil.h>

class Bitmap;
class Canvas;
class Interactor;

class WindowMgrHints {
public:
    WindowMgrHints();
    WindowMgrHints(XWMHints*);
    WindowMgrHints(Display*, Window);
    ~WindowMgrHints();

    void Set(Display*, Window);

    void Compute(
	Interactor*, Interactor* icon, Canvas*&,
	Interactor* leader, Canvas* leadercanvas
    );
    void WindowGroup(Interactor*, Canvas*);
    void IconPixmap(Bitmap*);
    void IconMask(Bitmap*);
    void IconWindow(Interactor*, Interactor* icon, Canvas*&);
    void IconPosition(Interactor*, const char*);
    void Size(
	Interactor*, Coord, Coord, int width, int height, boolean placed,
	XSizeHints&
    );
    void ClassHint(Interactor*, XClassHint&);
private:
    XWMHints* wm;

    int Squeeze(int a, int lower, int upper);
};

#endif
