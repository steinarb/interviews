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

// $Header: sltext.h,v 1.9 89/10/09 14:49:48 linton Exp $
// declares class TextSelection.

#ifndef sltext_h
#define sltext_h

#include "selection.h"

// Declare imported types.

class TextBuffer;

// A TextSelection draws one to several lines of text.

static const ClassId TEXTSELECTION = 2100;

class TextSelection : public Selection {
public:

    TextSelection(const char*, int, Graphic* = nil);
    TextSelection(istream&, State*);
    ~TextSelection();

    Graphic* Copy();
    boolean IsA(ClassId);

    const char* GetOriginal(int&);
    boolean ShapedBy(Coord, Coord, float);

protected:

    void draw(Canvas*, Graphic*);
    void drawClipped(Canvas*, Coord, Coord, Coord, Coord, Graphic*);
    void ReadjustSpacing(PFont*);

    void Init(const char*, int);
    const char* ReadTextData(istream&, int&);
    void WriteData(ostream&);
    const char* Filter(const char*, int);

    char* tbuf;			// stores the TextSelection's text
    TextBuffer* text;		// operates on the text
    int lineHt;			// stores previous line height

};

#endif
