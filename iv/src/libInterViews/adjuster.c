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
 * Implementation of Adjuster and derived classes.
 */

#include <InterViews/adjuster.h>
#include <InterViews/bitmap.h>
#include <InterViews/painter.h>
#include <InterViews/perspective.h>
#include <InterViews/sensor.h>
#include <InterViews/shape.h>

#include <InterViews/Bitmaps/enlargeHit.bm>
#include <InterViews/Bitmaps/enlargeMask.bm>
#include <InterViews/Bitmaps/enlarge.bm>
#include <InterViews/Bitmaps/reducerHit.bm>
#include <InterViews/Bitmaps/reducerMask.bm>
#include <InterViews/Bitmaps/reducer.bm>
#include <InterViews/Bitmaps/lmoverHit.bm>
#include <InterViews/Bitmaps/lmoverMask.bm>
#include <InterViews/Bitmaps/lmover.bm>
#include <InterViews/Bitmaps/rmoverHit.bm>
#include <InterViews/Bitmaps/rmoverMask.bm>
#include <InterViews/Bitmaps/rmover.bm>
#include <InterViews/Bitmaps/umoverHit.bm>
#include <InterViews/Bitmaps/umoverMask.bm>
#include <InterViews/Bitmaps/umover.bm>
#include <InterViews/Bitmaps/dmoverHit.bm>
#include <InterViews/Bitmaps/dmoverMask.bm>
#include <InterViews/Bitmaps/dmover.bm>

Adjuster::Adjuster (Interactor* i, int d) {
    Init(i, d);
}

Adjuster::Adjuster (const char* name, Interactor* i, int d) {
    SetInstance(name);
    Init(i, d);
}

Adjuster::Adjuster (Interactor* i, int d, Painter* p) : (nil, p) {
    Init(i, d);
}

void Adjuster::Init (Interactor* i, int d) {
    SetClassName("Adjuster");
    view = i;
    highlighted = false;
    delay = d;
    shown = new Perspective;
    plain = nil;
    hit = nil;
    mask = nil;
    input = new Sensor(onoffEvents);
    input->Catch(UpEvent);
    input->Catch(DownEvent);
}    

void Adjuster::Reconfig () {
    Painter* p = new Painter(output);
    p->Reference();
    Unref(output);
    output = p;
    shape->width = mask->Width();
    shape->height = mask->Height();
}

void Adjuster::AutoRepeat () {
    Event e;
    
    Poll(e);	// initialize event
    do {
	if (Check()) {
	    Read(e);
	    if (e.target == this) {
		switch (e.eventType) {
		    case OnEvent:
			Highlight(true);
			break;
		    case OffEvent:
			Highlight(false);
			break;
		    default:
			break;
		}
	    }		    
	} else if (highlighted) {
	    Flash();
	    AdjustView(e);
	    Sync();
	}
    } while (e.eventType != UpEvent);
}

void Adjuster::HandlePress () {
    Event e;
    
    do {
	Read(e);
	if (e.target == this) {
	    switch (e.eventType) {
		case OnEvent:
		    TimerOn();
		    Highlight(true);
		    break;
		case OffEvent:
		    TimerOff();
		    Highlight(false);
		    break;
		case UpEvent:
		    if (highlighted) {
			AdjustView(e);
		    }
		    break;
		case TimerEvent:
		    AutoRepeat();
		    return;
		default:
		    break;
	    }
	}
    } while (e.eventType != UpEvent);
}

void Adjuster::Flash () {
    Highlight(false);
    Highlight(true);
}

static const int USEC_PER_DELAY_UNIT = 100000;	    // delay unit = 1/10 secs

void Adjuster::TimerOn () {
    if (delay >= 0) {
	input->CatchTimer(0, delay * USEC_PER_DELAY_UNIT);
    }
}

void Adjuster::TimerOff () {
    input->Ignore(TimerEvent);
}

Adjuster::~Adjuster () {
    Unref(shown);
}

void Adjuster::Handle (Event& e) {
    if (e.eventType == DownEvent) {
	Highlight(true);
	TimerOn();
	if (delay == 0) {
	    AutoRepeat();
	} else {
	    HandlePress();
	}
	Highlight(false);
	TimerOff();
    }
}

void Adjuster::Redraw (Coord, Coord, Coord, Coord) {
    Coord x = (xmax+1 - mask->Width())/2;
    Coord y = (ymax+1 - mask->Height())/2;
    if (highlighted) {
        output->Stencil(canvas, x, y, hit, mask);
    } else {
        output->Stencil(canvas, x, y, plain, mask);
    }
}

void Adjuster::Reshape (Shape& s) {
    shape->Rigid(s.hshrink, s.hstretch, s.vshrink, s.vstretch);
}

void Adjuster::Highlight (boolean on) {
    if (highlighted != on) {
	highlighted = on;
        Draw();
    }
}

void Adjuster::AdjustView (Event&) {
    // nop default
}

Zoomer::Zoomer (Interactor* i, float f) : (i, NO_AUTOREPEAT) {
    Init(f);
}

Zoomer::Zoomer (
    const char* name, Interactor* i, float f
) : (name, i, NO_AUTOREPEAT) {
    Init(f);
}

Zoomer::Zoomer (
    Interactor* i, float f, Painter* p
) : (i, NO_AUTOREPEAT, p) {
    Init(f);
}

void Zoomer::Init (float f) {
    SetClassName("Zoomer");
    factor = f;
}

void Zoomer::AdjustView (Event&) {
    register Perspective* s = shown;
    Coord cx, cy;

    *s = *view->GetPerspective();
    cx = s->curx + s->curwidth/2;
    cy = s->cury + s->curheight/2;
    s->curwidth = round(float(s->curwidth) / factor);
    s->curheight = round(float(s->curheight) / factor);
    s->curx = cx - s->curwidth/2;
    s->cury = cy - s->curheight/2;
    view->Adjust(*s);    
}

inline Bitmap* MakeBitmap(void* bits, int width, int height) {
    Bitmap* b = new Bitmap(bits, width, height);
    b->Reference();
    return b;
}

static Bitmap* enlMask;
static Bitmap* enlPlain;
static Bitmap* enlHit;

Enlarger::Enlarger (Interactor* i) : (i, 2.0) {
    Init();
}

Enlarger::Enlarger (const char* name, Interactor* i) : (name, i, 2.0) {
    Init();
}

Enlarger::Enlarger (Interactor* i, Painter* p) : (i, 2.0, p) {
    Init();
}

void Enlarger::Init () {
    SetClassName("Enlarger");
    if (enlMask == nil) {
        enlMask = MakeBitmap(
            enlarger_mask_bits, enlarger_mask_width, enlarger_mask_height
        );
        enlPlain = MakeBitmap(
            enlarger_plain_bits, enlarger_plain_width, enlarger_plain_height
        );
        enlHit = MakeBitmap(
            enlarger_hit_bits, enlarger_hit_width, enlarger_hit_height
        );
    }
    mask = enlMask;
    plain = enlPlain;
    hit = enlHit;
    shape->Rigid(shape->width/2, hfil, shape->height/2);
}

static Bitmap* redMask;
static Bitmap* redPlain;
static Bitmap* redHit;

Reducer::Reducer (Interactor* i) : (i, 0.5) {
    Init();
}

Reducer::Reducer (const char* name, Interactor* i) : (name, i, 0.5) {
    Init();
}

Reducer::Reducer (Interactor* i, Painter* p) : (i, 0.5, p) {
    Init();
}

void Reducer::Init () {
    SetClassName("Reducer");
    if (redMask == nil) {
        redMask = MakeBitmap(
            reducer_mask_bits, reducer_mask_width, reducer_mask_height
        );
        redPlain = MakeBitmap(
            reducer_plain_bits, reducer_plain_width, reducer_plain_height
        );
        redHit = MakeBitmap(
            reducer_hit_bits, reducer_hit_width, reducer_hit_height
        );
    }
    mask = redMask;
    plain = redPlain;
    hit = redHit;
    shape->Rigid(shape->width/2, hfil, shape->height/2);
}

typedef enum MoveType { 
    MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN, MOVE_UNDEF
};

Mover::Mover (Interactor* i, int delay, int mt) : (i, delay) {
    Init(mt);
}

Mover::Mover (
    const char* name, Interactor* i, int delay, int mt
) : (name, i, delay) {
    Init(mt);
}

Mover::Mover (
    Interactor* i, int delay, int mt, Painter* p
) : (i, delay, p) {
    Init(mt);
}

void Mover::Init (int mt) {
    SetClassName("Mover");
    moveType = mt;
}

void Mover::AdjustView (Event& e) {
    register Perspective* s = shown;
    int amtx, amty;

    *s = *view->GetPerspective();
    amtx = e.shift ? s->lx : s->sx;
    amty = e.shift ? s->ly : s->sy;

    switch (moveType) {
	case MOVE_LEFT:	    s->curx -= amtx; break;
	case MOVE_RIGHT:    s->curx += amtx; break;
	case MOVE_UP:	    s->cury += amty; break;
	case MOVE_DOWN:	    s->cury -= amty; break;
	default:	    break;
    }
    view->Adjust(*s);    
}

static Bitmap* lmoverMask;
static Bitmap* lmoverPlain;
static Bitmap* lmoverHit;

LeftMover::LeftMover (Interactor* i, int delay) : (i, delay, MOVE_LEFT) {
    Init();
}

LeftMover::LeftMover (
    const char* name, Interactor* i, int delay
) : (name, i, delay, MOVE_LEFT) {
    Init();
}

LeftMover::LeftMover (
    Interactor* i, int delay, Painter* p
) : (i, delay, MOVE_LEFT, p) {
    Init();
};

void LeftMover::Init () {
    SetClassName("LeftMover");
    if (lmoverMask == nil) {
        lmoverMask = MakeBitmap(
            lmover_mask_bits, lmover_mask_width, lmover_mask_height
        );
        lmoverPlain = MakeBitmap(
            lmover_plain_bits, lmover_plain_width, lmover_plain_height
        );
        lmoverHit = MakeBitmap(
            lmover_hit_bits, lmover_hit_width, lmover_hit_height
        );
    }
    mask = lmoverMask;
    plain = lmoverPlain;
    hit = lmoverHit;
    shape->Rigid(shape->width/2, 0, shape->height/2, vfil);
}

static Bitmap* rmoverMask;
static Bitmap* rmoverPlain;
static Bitmap* rmoverHit;

RightMover::RightMover (Interactor* i, int delay) : (i, delay, MOVE_RIGHT) {
    Init();
}

RightMover::RightMover (
    const char* name, Interactor* i, int delay
) : (name, i, delay, MOVE_RIGHT) {
    Init();
}

RightMover::RightMover (
    Interactor* i, int delay, Painter* p
) : (i, delay, MOVE_RIGHT, p) {
    Init();
};

void RightMover::Init () {
    SetClassName("RightMover");
    if (rmoverMask == nil) {
        rmoverMask = MakeBitmap(
            rmover_mask_bits, rmover_mask_width, rmover_mask_height
        );
        rmoverPlain = MakeBitmap(
            rmover_plain_bits, rmover_plain_width, rmover_plain_height
        );
        rmoverHit = MakeBitmap(
            rmover_hit_bits, rmover_hit_width, rmover_hit_height
        );
    }
    mask = rmoverMask;
    plain = rmoverPlain;
    hit = rmoverHit;
    shape->Rigid(shape->width/2, 0, shape->height/2, vfil);
}

static Bitmap* umoverMask;
static Bitmap* umoverPlain;
static Bitmap* umoverHit;

UpMover::UpMover (Interactor* i, int delay) : (i, delay, MOVE_UP) {
    Init();
}

UpMover::UpMover (
    const char* name, Interactor* i, int delay
) : (name, i, delay, MOVE_UP) {
    Init();
}

UpMover::UpMover (
    Interactor* i, int delay, Painter* p
) : (i, delay, MOVE_UP, p) {
    Init();
};

void UpMover::Init () {
    SetClassName("UpMover");
    if (umoverMask == nil) {
        umoverMask = MakeBitmap(
            umover_mask_bits, umover_mask_width, umover_mask_height
        );
        umoverPlain = MakeBitmap(
            umover_plain_bits, umover_plain_width, umover_plain_height
        );
        umoverHit = MakeBitmap(
            umover_hit_bits, umover_hit_width, umover_hit_height
        );
    }
    mask = umoverMask;
    plain = umoverPlain;
    hit = umoverHit;
    shape->Rigid(shape->width/2, hfil, shape->height/2);
}

static Bitmap* dmoverMask;
static Bitmap* dmoverPlain;
static Bitmap* dmoverHit;

DownMover::DownMover (Interactor* i, int delay) : (i, delay, MOVE_DOWN) {
    Init();
}

DownMover::DownMover (
    const char* name, Interactor* i, int delay
) : (name, i, delay, MOVE_DOWN) {
    Init();
}

DownMover::DownMover (
    Interactor* i, int delay, Painter* p
) : (i, delay, MOVE_DOWN, p) {
    Init();
};

void DownMover::Init () {
    SetClassName("DownMover");
    if (dmoverMask == nil) {
        dmoverMask = MakeBitmap(
            dmover_mask_bits, dmover_mask_width, dmover_mask_height
        );
        dmoverPlain = MakeBitmap(
            dmover_plain_bits, dmover_plain_width, dmover_plain_height
        );
        dmoverHit = MakeBitmap(
            dmover_hit_bits, dmover_hit_width, dmover_hit_height
        );
    }
    mask = dmoverMask;
    plain = dmoverPlain;
    hit = dmoverHit;
    shape->Rigid(shape->width/2, hfil, shape->height/2);
}
