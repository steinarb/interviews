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

// $Header: highlighter.c,v 1.11 89/10/09 14:48:02 linton Exp $
// implements classes HighlighterParent and Highlighter.

#include "highlighter.h"
#include <InterViews/painter.h>

// HighlighterParent starts with no highlight painter.

HighlighterParent::HighlighterParent () {
    highlight = nil;
}

// Free storage allocated for the highlight painter.

HighlighterParent::~HighlighterParent () {
    Unref(highlight);
}

// SameOutputAs compares the given painter to our output painter so
// our interior Highlighters can decide if they can share our
// highlight painter.

boolean HighlighterParent::SameOutputAs (Painter* out) {
    return out == output;
}

// GetHighlightPainter creates our highlight painter if we don't have
// one yet and returns it so all of our interior Highlighters can
// share it as well as our output painter which they inherit
// automatically.  We can't just create highlight in Reconfig because
// our interior Highlighters execute their Reconfig before we do, so
// we create it here (once) when they call us from their Reconfig.

Painter* HighlighterParent::GetHighlightPainter () {
    if (highlight == nil) {
	highlight = new Painter(output);
	highlight->Reference();
	highlight->SetColors(output->GetBgColor(), output->GetFgColor());
    }
    return highlight;
}

// Highlighter starts off unhighlighted with no HighlighterParent yet.

Highlighter::Highlighter () {
    hparent = nil;
    highlighted = false;
    highlight = nil;
    normal = nil;
}

// Free storage allocated for the highlight painter.

Highlighter::~Highlighter () {
    output = normal;
    Unref(highlight);
}

// SetHighlighterParent gives us a HighlighterParent.

void Highlighter::SetHighlighterParent (HighlighterParent* hp) {
    hparent = hp;
}

// Highlight exchanges our painter and draws us unless we
// don't have a canvas so a panel can highlight us before the panel's
// inserted and a menu can unhighlight us after the menu's removed.

void Highlighter::Highlight (boolean on) {
    highlighted = on;
    output = on ? highlight : normal;
    if (canvas != nil) {
	Redraw(0, 0, xmax, ymax);
    }
}

// Reconfig initializes our highlight painter if necessary by getting
// it from our HighlighterParent if possible or else creating a new
// painter.  Then Reconfig switches to the appropriate painter.

void Highlighter::Reconfig () {
    Interactor::Reconfig();
    if (output != highlight && output != normal) {
	Unref(highlight);
	if (hparent != nil && hparent->SameOutputAs(output)) {
	    highlight = hparent->GetHighlightPainter();
	    highlight->Reference();
	} else {		// bite the bullet
	    highlight = new Painter(output);
	    highlight->Reference();
	    highlight->SetColors(output->GetBgColor(), output->GetFgColor());
	}
	normal = output;
    }
    output = highlighted ? highlight : normal;
}
