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
 * a Button which cycles through multiple choices
 */

#include "cyclebutton.h"

#include <InterViews/bitmap.h>
#include <InterViews/font.h>
#include <InterViews/painter.h>
#include <InterViews/shape.h>

static const int cycle_width = 11;
static const int cycle_height = 11;
static char cycle_mask[] = {
   0x78, 0x00, 0xfc, 0x05, 0xfe, 0x07, 0xfe, 0x07, 0xff, 0x07, 0xff, 0x07,
   0xff, 0x07, 0xff, 0x03, 0xff, 0x03, 0xfd, 0x01, 0xf0, 0x00};
static char cycle_plain[] = {
   0x78, 0x00, 0x84, 0x05, 0x00, 0x07, 0x80, 0x04, 0x01, 0x05, 0x03, 0x06,
   0x05, 0x04, 0x09, 0x00, 0x07, 0x00, 0x0d, 0x01, 0xf0, 0x00};
static char cycle_hit[] = {
   0x78, 0x00, 0x84, 0x05, 0x00, 0x07, 0x80, 0x07, 0x01, 0x07, 0x03, 0x06,
   0x07, 0x04, 0x0f, 0x00, 0x07, 0x00, 0x0d, 0x01, 0xf0, 0x00};

static Bitmap* CycleMask;
static Bitmap* CyclePlain;
static Bitmap* CycleHit;

static const int Separator = 3;

CycleButton::CycleButton (
    ButtonState* subject, CycleButtonChoice* choices
)  : Button(subject, nil) {
    Init(choices);
}

CycleButton::CycleButton (
    const char* name, ButtonState* subject, CycleButtonChoice* choices
) : Button(subject, nil) {
    SetInstance(name);
    Init(choices);
}

CycleButton::~CycleButton () { }

void CycleButton::Init (CycleButtonChoice* c) {
    SetClassName("CycleButton");
    if (CycleMask == nil) {
        CycleMask = new Bitmap(cycle_mask, cycle_width, cycle_height);
        CyclePlain = new Bitmap(cycle_plain, cycle_width, cycle_height);
        CycleHit = new Bitmap(cycle_hit, cycle_width, cycle_height);
    }
    choices = c;
    for (count_ = 0; choices[count_].label != nil; ++count_) { }
    current = 0;
}

void CycleButton::Reconfig () {
    const Font* font = output->GetFont();
    int width = 0;
    for (int i = 0; i < count_; ++i) {
        width = max(width, font->Width(choices[i].label));
    }
    Button::Reconfig();
    shape->Rect(width + cycle_width + Separator, font->Height());
    shape->Rigid();
}

void CycleButton::Redraw (Coord l, Coord b, Coord r, Coord t) {
    Coord textx = cycle_width + Separator;
    Coord texty = (ymax + 1 - output->GetFont()->Height()) / 2;
    output->ClearRect(canvas, l, b, r, t);
    output->Text(canvas, choices[current].label, textx, texty);
    Refresh();
}

void CycleButton::Refresh () {
    Coord bitsx = 0;
    Coord bitsy = (ymax + 1 - cycle_height)/2;
    if (!hit) {
        output->Stencil(canvas, bitsx, bitsy, CyclePlain, CycleMask);
    } else {
        output->Stencil(canvas, bitsx, bitsy, CycleHit, CycleMask);
    }
}

void CycleButton::Press () {
    subject->SetValue(choices[(current + 1) % count_].value);
}

void CycleButton::Update() {
    void* v;
    subject->GetValue(v);
    if (v != choices[current].value) {
	for (int i = 0; i < count_; ++i) {
	    if (v == choices[i].value) {
		current = i;
		break;
	    }
	}
	Draw();
    }
}
