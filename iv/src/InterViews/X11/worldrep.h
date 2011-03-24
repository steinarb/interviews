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
 * X11-dependent World representation.
 */

#ifndef worldrep_h
#define worldrep_h

#include <InterViews/X11/Xlib.h>

class BitmapTable;
class InteractorTable;
class WorldRep;

extern WorldRep* _world;

extern int saved_argc;
extern char** saved_argv;

/*
 * The current WorldView instance is only relevant to
 * window manager applications.
 */
extern class WorldView* _worldview;

enum TxFontsOption {
    TxFontsDefault, TxFontsOff, TxFontsOn, TxFontsCache
};

enum TxImagesOption {
    TxImagesDefault, TxImagesAuto, TxImagesDest, TxImagesSource
};

enum DashOption {
    DashDefault, DashNone, DashThin, DashAll
};

class WorldRep {
public:
    char* hostname() { return _host[0] == '\0' ? gethostname() : _host; }
    Display* display() { return _display; }
    int screen() { return _screen; }
    Window root() { return _root; }
    Visual* visual() { return _visual; }
    XColormap cmap() { return _cmap; }
    int xor() { return _xor; }
    TxFontsOption txfonts() { return _txfonts; }
    TxImagesOption tximages() { return _tximages; }
    DashOption dash() { return _dash; }
    InteractorTable* itable() { return _itable; }

    /* for caching transformed fonts */
    BitmapTable* _btable;
    BitmapTable* _txtable;
private:
    friend class World;

    static char _host[100];
    Display* _display;
    int _screen;
    Window _root;
    Visual* _visual;
    XColormap _cmap;
    int _xor;
    TxFontsOption _txfonts;
    TxImagesOption _tximages;
    DashOption _dash;
    InteractorTable* _itable;

    char* gethostname();
};

#endif
