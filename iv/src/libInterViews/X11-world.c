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
 * X11-dependent world code
 */

#include "btable.h"
#include "itable.h"
#include <InterViews/bitmap.h>
#include <InterViews/canvas.h>
#include <InterViews/color.h>
#include <InterViews/font.h>
#include <InterViews/painter.h>
#include <InterViews/interactor.h>
#include <InterViews/shape.h>
#include <InterViews/world.h>
#include <InterViews/X11/worldrep.h>
#include <InterViews/X11/Xlib.h>
#include <InterViews/X11/Xutil.h>
#include <os/host.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void InitSensors(), InitCursors(Color*, Color*);

WorldRep* _world;

int saved_argc;
char** saved_argv;

#if defined(hpux)
#include <sys/utsname.h>
#endif

char* WorldRep::gethostname () {
#if defined(hpux)
    struct utsname name;
    uname(&name);
    strncpy(_host, name.nodename, sizeof(_host));
#else
    ::gethostname(_host, sizeof(_host));
#endif
    return _host;
}

void World::Init (const char* device) {
    rep = new WorldRep;
    rep->_display = XOpenDisplay(device);
    if (rep->display() == nil) {
	fprintf(
	    stderr, "fatal error: can't open display '%s'\n",
	    XDisplayName(device)
	);
	exit(1);
    }
    rep->_screen = DefaultScreen(rep->display());
    rep->_root = RootWindow(rep->display(), rep->screen());
    rep->_visual = DefaultVisual(rep->display(), rep->screen());
    rep->_cmap = DefaultColormap(rep->display(), rep->screen());
    rep->_xor = 1;
    rep->_txfonts = TxFontsDefault;
    rep->_tximages = TxImagesDefault;
    rep->_dash = DashDefault;
    SetCurrent();
    rep->_itable = new InteractorTable(2048);
    rep->_itable->Insert((void*)rep->root(), this);
    rep->_btable = nil;
    rep->_txtable = nil;
    canvas = new Canvas((void*)rep->root());
    canvas->width = DisplayWidth(rep->display(), rep->screen());
    canvas->height = DisplayHeight(rep->display(), rep->screen());
    canvas->status = CanvasMapped;
    xmax = canvas->width - 1;
    ymax = canvas->height - 1;
    double pixmm = (
	double(canvas->width) /
	double(DisplayWidthMM(rep->display(), rep->screen()))
    );
    cm = round(10*pixmm);
    inch = round(25.4*pixmm);
    inches = inch;
    point = inch/72.27;
    points = point;

    black = new Color("black");
    if (!black->Valid()) {
        Unref(black);
        black = new Color((int)BlackPixel(rep->display(), rep->screen()));
    }
    black->Reference();
    white = new Color("white");
    if (!white->Valid()) {
        Unref(white);
        white = new Color((int)WhitePixel(rep->display(), rep->screen()));
    }
    white->Reference();
    stdfont = new Font("fixed");
    stdfont->Reference();
    stdpaint = new Painter();
    stdpaint->Reference();
}

static inline unsigned int MSB (unsigned long i) {
    return (i ^ (i>>1)) & i;
}

void World::FinishInit () {
    const char* xorpixel = GetAttribute("RubberbandPixelHint");
    char buffer[256];
    if (xorpixel != nil) {
        sprintf(buffer, "RubberbandPixel:%s", xorpixel);
        SetHint(buffer);
    } else if (rep->visual()->c_class != DirectColor) {
        rep->_xor = black->PixelValue() ^ white->PixelValue();
    } else {
        rep->_xor = (
            MSB(rep->visual()->red_mask) |
            MSB(rep->visual()->green_mask) |
            MSB(rep->visual()->blue_mask)
        );
    }
    const char* txfonts = GetAttribute("TransformFontsHint");
    if (txfonts != nil) {
        sprintf(buffer, "TransformFonts:%s", txfonts);
        SetHint(buffer);
    }
    const char* tximages = GetAttribute("TransformImagesHint");
    if (tximages != nil) {
        sprintf(buffer, "TransformImages:%s", tximages);
        SetHint(buffer);
    }
    const char* dash = GetAttribute("DashHint");
    if (dash != nil) {
        sprintf(buffer, "Dash:%s", dash);
        SetHint(buffer);
    }
    SetCurrent();

    RootConfig();
    InitSensors();
    InitCursors(output->GetFgColor(), output->GetBgColor());
}

World::~World () {
    XCloseDisplay(rep->display());
    delete rep->_itable;
    delete rep;
}

void World::SaveCommandLine (int argc, char* argv[]) {
    saved_argc = argc;
    saved_argv = new char*[argc + 1];

    for (int i = 0 ; i < argc ; i++) {
	saved_argv[i] = argv[i];
    }
    saved_argv[i] = nil;
}

const char* World::UserDefaults () {
    return rep->display()->xdefaults;
}

void World::FinishInsert (Interactor*) {
    /* nothing else to do */
}

void World::DoChange (Interactor* i) {
    Canvas* c = i->canvas;
    if (c != nil && c->status == CanvasMapped) {
	Shape* s = i->GetShape();
	if (c->width != s->width || c->height != s->height) {
	    XResizeWindow(rep->display(), (Window)c->id, s->width, s->height);
	} else {
	    i->Resize();
	}
    }
}

void World::DoRemove (Interactor*) {
    XFlush(rep->display());
}

int World::Fileno () {
    return ConnectionNumber(rep->display());
}

void World::SetCurrent () {
    _world = rep;
}

void World::SetRoot (void* r) {
    rep->_root = (Window)r;
}

void World::SetScreen (int n) {
    rep->_screen = n;
}

int World::NPlanes () {
    return DisplayPlanes(rep->display(), rep->screen());
}

int World::NButtons () {
    return 3;
}

int World::PixelSize () {
    return BitmapPad(rep->display());
}

const char* World::GetDefault (const char* name) {
    return GetAttribute(name);
}

const char* World::GetGlobalDefault (const char* name) {
    return GetAttribute(name);
}

unsigned World::ParseGeometry (
    const char* spec, Coord& x, Coord& y, unsigned int& w, unsigned int &h
) {
    return XParseGeometry(spec, &x, &y, &w, &h);
}

static void FlushCharBitmaps() {
    if (_world->_btable != nil) {
        /* not implemented, but should go something like this
        Bitmap* b;
        while (!_world->_btable->Empty()) {
            b = _world->_btable->RemoveEntry();
            Unref(b);
        }
        */
        delete _world->_btable;
        _world->_btable = nil;
    }
    if (_world->_txtable != nil) {
        /* not implemented, but should go something like this
        Bitmap* b;
        while (!_world->_txtable->Empty()) {
            b = _world->_txtable->RemoveEntry();
            Unref(b);
        }
        */
        delete _world->_txtable;
        _world->_txtable = nil;
    }
}

void World::SetHint (const char* hint) {
    char buf[256];
    char value[256];
    strcpy(buf, hint);
    if (sscanf(buf, "RubberbandPixel:%s", value) == 1) {
        sscanf(value, "%d", &rep->_xor);
    } else if (sscanf(buf, "TransformFonts:%s", value) == 1) {
        if (strcmp(value, "off") == 0 || strcmp(value, "Off") == 0) {
            rep->_txfonts = TxFontsOff;
        } else if (strcmp(value, "on") == 0 || strcmp(value, "On") == 0) {
            rep->_txfonts = TxFontsOn;
        } else if (strcmp(value, "cache")==0 || strcmp(value, "Cache")==0) {
            rep->_txfonts = TxFontsCache;
        } else {
            rep->_txfonts = TxFontsDefault;
        }
        if (rep->_txfonts != TxFontsCache) {
            FlushCharBitmaps();
        }
    } else if (sscanf(buf, "TransformImages:%s", value) == 1) {
        if (strcmp(value, "auto") == 0 || strcmp(value, "Auto") == 0) {
            rep->_tximages = TxImagesAuto;
        } else if (
            strcmp(value, "destination") == 0
            || strcmp(value, "Destination") == 0
        ) {
            rep->_tximages = TxImagesDest;
        } else if (strcmp(value, "source")==0 || strcmp(value, "Source")==0) {
            rep->_tximages = TxImagesSource;
        } else {
            rep->_tximages = TxImagesDefault;
        }
    } else if (sscanf(buf, "Dash:%s", value) == 1) {
        if (strcmp(value, "none") == 0 || strcmp(value, "None") == 0) {
            rep->_dash = DashNone;
        } else if (strcmp(value, "thin") == 0 || strcmp(value, "Thin") == 0) {
            rep->_dash = DashThin;
        } else if (strcmp(value, "all") == 0 || strcmp(value, "All") == 0) {
            rep->_dash = DashAll;
        } else {
            rep->_dash = DashDefault;
        }
    }
}

void World::RingBell (int v) {
    if (v > 100) {
	XBell(rep->display(), 100);
    } else if (v >= 0) {
	XBell(rep->display(), v);
    }
}

void World::SetKeyClick (int v) {
    XKeyboardControl k;

    k.key_click_percent = v;
    XChangeKeyboardControl(rep->display(), KBKeyClickPercent, &k);
}

void World::SetAutoRepeat (boolean b) {
    if (b) {
	XAutoRepeatOn(rep->display());
    } else {
	XAutoRepeatOff(rep->display());
    }
}

void World::SetFeedback (int t, int s) {
    XChangePointerControl(rep->display(), True, True, s, 1, t);
}
