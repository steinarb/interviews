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

// $Header: highlighter.h,v 1.10 89/10/09 14:48:04 linton Exp $
// declares classes HighlighterParent and Highlighter.

#ifndef highlighter_h
#define highlighter_h

#include <InterViews/scene.h>

// A HighlighterParent creates a highlight painter for its interior
// Highlighters to share.

class HighlighterParent : public MonoScene {
public:

    HighlighterParent();
    ~HighlighterParent();

    boolean SameOutputAs(Painter*);
    Painter* GetHighlightPainter();

protected:

    Painter* highlight;		// stores painter to give interior Highlighters

};

// A Highlighter draws itself and highlights or unhighlights itself on
// command.

class Highlighter : public Interactor {
public:

    Highlighter();
    ~Highlighter();

    void SetHighlighterParent(HighlighterParent*);

    void Highlight(boolean on);

protected:

    void Reconfig();

    HighlighterParent* hparent;	// gives us highlight painter if it's nonnil
    boolean highlighted;	// stores true if we should be highlighted
    Painter* highlight;		// draws us with reversed colors
    Painter* normal;		// draws us with normal colors

};

#endif
