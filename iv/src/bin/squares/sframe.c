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
 * Implementation of the frame around a squares view.
 */

#if defined(__cplusplus)
#include "sframe.h"
#include "squares.h"
#include "metaview.h"
#include "view.h"
#endif
#include <InterViews/banner.h>
#include <InterViews/border.h>
#include <InterViews/box.h>
#include <InterViews/button.h>
#include <InterViews/event.h>
#include <InterViews/frame.h>
#include <InterViews/glue.h>
#include <InterViews/menu.h>
#include <InterViews/panner.h>
#include <InterViews/scroller.h>
#include <InterViews/sensor.h>
#include <InterViews/shape.h>
#include <InterViews/tray.h>
#include <InterViews/viewport.h>
#include <InterViews/world.h>
#include <stdlib.h>

class SquaresFrame;

typedef void (SquaresFrame::*MenuFunc)();

class Command : public MenuItem {
public:
    Command(const char*, Alignment, SquaresFrame*, MenuFunc);

    virtual void Do();
private:
    SquaresFrame* frame;
    MenuFunc func;
};

Command::Command(
    const char* str, Alignment al, SquaresFrame* s, MenuFunc f
) : (str, al) {
    frame = s;
    func = f;
}

void Command::Do() {
    (frame->*func)();
}

struct MenuInfo {
    const char* str;
    MenuFunc func;
};

#if !defined(__cplusplus)
#include "sframe.h"
#include "squares.h"
#include "metaview.h"
#include "view.h"
#endif

static MenuInfo mainmenu[] = {
    { "add square", &SquaresFrame::AddSquare },
    { "new view", &SquaresFrame::NewView },
    { "view setup", &SquaresFrame::ViewSetup },
    { "close", &SquaresFrame::Close },
    { "quit", &SquaresFrame::Quit },
    { nil }
};

static MenuInfo adjustmenu[] = {
    { "zoom in", &SquaresFrame::ZoomIn },
    { "zoom out", &SquaresFrame::ZoomOut },
    { "normal size", &SquaresFrame::NormalSize },
    { "center view", &SquaresFrame::CenterView },
    { nil }
};
    
static MenuInfo quitmenu[] = {
    { "yes, quit", &SquaresFrame::YesQuit },
    { "no, don't quit", &SquaresFrame::NoQuit },
    { nil }
};

SquaresFrame::SquaresFrame (SquaresFrame* f) {
    view = new SquaresView(f->view->subject);
    Init();
    style = new SquaresMetaView(f->style);
    MakeFrame();
}

SquaresFrame::SquaresFrame (SquaresView* v) {
    const char* a;

    view = v;
    Init();
    style = new SquaresMetaView;
    a = GetAttribute("panner");
    style->type = (a == nil) ? AdjustByScrollers : AdjustByPanner;
    a = GetAttribute("adjustersize");
    if (*a == 'm') {
	style->size = Medium;
    } else if (*a == 'l') {
	style->size = Large;
    } else {
	style->size = Small;
    }
    style->right = (GetAttribute("left") == nil);
    style->below = (GetAttribute("above") == nil);
    style->hscroll = (GetAttribute("!hscroll") == nil);
    style->vscroll = (GetAttribute("!vscroll") == nil);
    MakeFrame();
}

void SquaresFrame::Init () {
    SetClassName("SquaresFrame");
    menu = MakeMenu(new PopupMenu, mainmenu);
    adjust = MakeMenu(new PopupMenu, adjustmenu);
    quit = MakeMenu(new PopupMenu, quitmenu);
    Unref(input);
    input = updownEvents;
    input->Reference();
    viewport = new Viewport(new Frame(view));
    Propagate(false);
    ++nviews;
}

SquaresFrame::~SquaresFrame () {
    delete menu;
    delete quit;
    delete style;
}

Menu* SquaresFrame::MakeMenu(Menu* menu, MenuInfo* item) {
    for (MenuInfo* i = item; i->str != nil; i++) {
	menu->Include(new Command(i->str, Center, this, i->func));
    }
    return menu;
}

void SquaresFrame::MakeFrame () {
    Scene* p = viewport->Parent();
    Interactor* interior;

    if (p != nil) {
	p->Remove(viewport);
    }
    if (style->type == AdjustByPanner) {
	interior = PannerFrameInterior();
    } else if (style->type == AdjustByScrollers) {
	interior = ScrollerFrameInterior();
    }
    Insert(interior);
}

Interactor* SquaresFrame::ScrollerFrameInterior () {
    Interactor* v = viewport;
    Tray* t = new Tray;
    Interactor* hs, *vs;
    Border* hb, *vb;
    int size;

    if (style->size == Small) {
	size = round(0.15*inches);
    } else if (style->size == Medium) {
	size = round(0.20*inches);
    } else if (style->size == Large) {
	size = round(0.25*inches);
    }

    if (style->vscroll) {
	vs = new VScroller(viewport, size);
	vb = new VBorder;

	if (style->right) {
	    t->HBox(t, v, vb, vs, t);
	} else {
	    t->HBox(t, vs, vb, v, t);
	}
	t->VBox(t, vb, t);
    } else {
	t->HBox(t, v, t);
    }

    if (style->hscroll) {
	hs = new HScroller(viewport, size);
	hb = new HBorder;

	if (style->below) {
	    t->VBox(t, v, hb, hs, t);
	} else {
	    t->VBox(t, hs, hb, v, t);
	}
	t->HBox(t, hb, t);
    } else {
	t->VBox(t, v, t);
    }
    
    if (style->vscroll && style->hscroll) {
	if (style->below) {
	    t->VBox(t, vs, hb);
	} else {
	    t->VBox(hb, vs, t);
	}
	if (style->right) {
	    t->HBox(t, hs, vb);
	} else {
	    t->HBox(vb, hs, t);
	}

    } else if (style->vscroll) {
	t->VBox(t, vs, t);

    } else if (style->hscroll) {
	t->HBox(t, hs, t);
    }
    return t;
}

Interactor* SquaresFrame::PannerFrameInterior () {
    Interactor* v = viewport;
    Tray* t = new Tray(v);
    int size;

    if (style->size == Small) {
	size = 0;
    } else if (style->size == Medium) {
	size = round(0.75*inches);
    } else if (style->size == Large) {
	size = round(1.0*inches);
    }
    t->Align(style->align, new Frame(new Panner(v, size)));
    t->Propagate(false);
    return t;
}

void SquaresFrame::Handle (Event& e) {
    if (e.eventType == DownEvent) {
	if (e.button == RIGHTMOUSE) {
	    adjust->Popup(e);
	} else {
	    menu->Popup(e);
	}
    }
}

void SquaresFrame::AddSquare() {
    view->subject->Add();
}

void SquaresFrame::NewView() {
    GetWorld()->InsertToplevel(new SquaresFrame(this), this);
}

void SquaresFrame::ViewSetup() {
    Event e;
    Poll(e);
    e.target = this;
    if (style->Popup(e)) {
	MakeFrame();
	Change();
    }
}

void SquaresFrame::Close() {
    if (nviews == 1) {
	Event e;
	Poll(e);
	quit->Popup(e);
    } else {
	--nviews;
	delete this;
    }
}

void SquaresFrame::Quit() {
    Event e;
    Poll(e);
    quit->Popup(e);
}

void SquaresFrame::ZoomIn() {
    viewport->ZoomBy(2.0, 2.0);
}

void SquaresFrame::ZoomOut() {
    viewport->ZoomBy(0.5, 0.5);
}

void SquaresFrame::NormalSize() {
    viewport->ZoomTo(1.0, 1.0);
}

void SquaresFrame::CenterView() {
    viewport->ScrollTo(0.5, 0.5);
}

void SquaresFrame::YesQuit() {
    exit(0);
}

void SquaresFrame::NoQuit() {
    /* nop */
}
