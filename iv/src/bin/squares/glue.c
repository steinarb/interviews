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
 * Special glue for demonstration purposes.
 * Unlike normal glue, we display the glue shape and actual size
 * when a button is clicked.
 */

#include <InterViews/interactor.h>
#include <InterViews/sensor.h>
#include <InterViews/shape.h>
#include <InterViews/painter.h>
#include <stdio.h>

class Glue : public Interactor {
protected:
    Glue();
    Glue(const char*);
    Glue(Painter* bg);

    virtual void Redraw(Coord, Coord, Coord, Coord);
private:
    void Init();
};

class HGlue : public Glue {
public:
    HGlue(int natural = 0, int stretch = hfil);
    HGlue(const char*, int natural = 0, int stretch = hfil);
    HGlue(int natural, int shrink, int stretch);
    HGlue(const char*, int natural, int shrink, int stretch);
    HGlue(Painter* bg, int natural = 0, int stretch = hfil);
    HGlue(Painter* bg, int natural, int shrink, int stretch);

    virtual void Handle(Event&);
private:
    void Init(int nat, int shrink, int stretch);
};

class VGlue : public Glue {
public:
    VGlue(int natural = 0, int stretch = vfil);
    VGlue(const char*, int natural = 0, int stretch = vfil);
    VGlue(int natural, int shrink, int stretch);
    VGlue(const char*, int natural, int shrink, int stretch);
    VGlue(Painter* bg, int natural = 0, int stretch = vfil);
    VGlue(Painter* bg, int natural, int shrink, int stretch);

    virtual void Handle(Event&);
private:
    void Init(int nat, int shrink, int stretch);
};

Glue::Glue () {
    Init();
}

Glue::Glue (const char* name) {
    SetInstance(name);
    Init();
}

Glue::Glue (Painter* bg) : (nil, bg) {
    Init();
}

void Glue::Init () {
    SetClassName("Glue");
    input = updownEvents;
    input->Reference();
}

void Glue::Redraw (Coord x1, Coord y1, Coord x2, Coord y2) {
    output->ClearRect(canvas, x1, y1, x2, y2);
}

HGlue::HGlue (int nat, int str) {
    Init(nat, nat, str);
}

HGlue::HGlue (const char* name, int nat, int str) : (name) {
    Init(nat, nat, str);
}

HGlue::HGlue (int nat, int shr, int str) {
    Init(nat, shr, str);
}

HGlue::HGlue (const char* name, int nat, int shr, int str) : (name) {
    Init(nat, shr, str);
}

HGlue::HGlue (Painter* bg, int nat, int str) : (bg) {
    Init(nat, nat, str);
}

HGlue::HGlue (Painter* bg, int nat, int shr, int str) : (bg) {
    Init(nat, shr, str);
}

void HGlue::Init (int nat, int shr, int str) {
    SetClassName("HGlue");
    shape->width = nat;
    shape->height = 0;
    shape->Rigid(shr, str, vfil, vfil);
}

VGlue::VGlue (int nat, int str) {
    Init(nat, nat, str);
}

VGlue::VGlue (const char* name, int nat, int str) : (name) {
    Init(nat, nat, str);
}

VGlue::VGlue (int nat, int shr, int str) {
    Init(nat, shr, str);
}

VGlue::VGlue (const char* name, int nat, int shr, int str) : (name) {
    Init(nat, shr, str);
}

VGlue::VGlue (Painter* bg, int nat, int str) : (bg) {
    Init(nat, nat, str);
}

VGlue::VGlue (Painter* bg, int nat, int shr, int str) : (bg) {
    Init(nat, shr, str);
}

void VGlue::Init (int nat, int shr, int str) {
    SetClassName("VGlue");
    shape->width = 0;
    shape->height = nat;
    shape->Rigid(hfil, hfil, shr, str);
}

/*
 * Differences from standard glue -- handle events differently.
 */

void HGlue::Handle (Event& e) {
    if (e.eventType == DownEvent) {
	char buf[100];

	sprintf(
	    buf, "HGlue[%d], natural (%d,%d,%d)",
	    xmax+1, shape->width, shape->hstretch, shape->hshrink
	);
	output->SetColors(output->GetBgColor(), output->GetFgColor());
	output->ClearRect(canvas, 0, 0, xmax, ymax);
	output->Text(canvas, buf, 0, 0);
	output->SetColors(output->GetBgColor(), output->GetFgColor());
    } else if (e.eventType == UpEvent) {
	Draw();
    }
}

void VGlue::Handle (Event& e) {
    if (e.eventType == DownEvent) {
	char buf[100];

	sprintf(
	    buf, "VGlue[%d], natural (%d,%d,%d)",
	    ymax+1, shape->height, shape->vstretch, shape->vshrink
	);
	output->SetColors(output->GetBgColor(), output->GetFgColor());
	output->ClearRect(canvas, 0, 0, xmax, ymax);
	output->Text(canvas, buf, 0, 0);
	output->SetColors(output->GetBgColor(), output->GetFgColor());
    } else if (e.eventType == UpEvent) {
	Draw();
    }
}
