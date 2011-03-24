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

// $Header: sltext.c,v 1.13 89/10/09 14:49:47 linton Exp $
// implements class TextSelection.

#include "ipaint.h"
#include "istring.h"
#include "sltext.h"
#include <InterViews/Graphic/label.h>
#include <InterViews/textbuffer.h>
#include <bstring.h>
#include <stream.h>

// Both ReadTextData and Filter use this big static buffer.

static const int SBUFSIZE = 10000;
static char sbuf[SBUFSIZE];

// TextSelection gets passed its graphic state and text.

TextSelection::TextSelection (const char* orig, int len, Graphic* gs) : (gs) {
    Init(orig, len);
}

// TextSelection reads its graphic state and text from a file.

TextSelection::TextSelection (istream& from, State* state) : (nil) {
    ReadTextGS(from, state);
    int len;
    const char* orig = ReadTextData(from, len);
    Init(orig, len);
}

// Init copies the original text and appends Labels for each line of
// text to the TextSelection.

void TextSelection::Init (const char* orig, int len) {
    int tbufsize = max(len, 1);
    tbuf = new char[tbufsize];
    bcopy(orig, tbuf, len);
    text = new TextBuffer(tbuf, len, tbufsize);
    lineHt = 0;

    int lines = text->Height();
    for (int i = 0; i < lines; ++i) {
	int bol = text->LineIndex(i);
	int eol = text->EndOfLine(bol);
	/* need this crock for now because Label(t, 0) is buggy */
	if (eol - bol > 0) {
	    Append(new Label(text->Text(bol, eol), eol - bol));
	} else {
	    Append(new Label(""));
	}
    }
    ReadjustSpacing(GetFont());
}

// Free storage allocated for the text in the TextSelection.

TextSelection::~TextSelection () {
    delete text;
    delete tbuf;
}

// Copy returns a copy of the TextSelection.

Graphic* TextSelection::Copy () {
    return new TextSelection(text->Text(), text->Length(), this);
}

// IsA returns true if the TextSelection is a TextSelection so Editor
// can identify TextSelections and edit them differently.

boolean TextSelection::IsA (ClassId id) {
    return id == TEXTSELECTION || Picture::IsA(id);
}

// GetOriginal returns a pointer to the TextSelection's text.

const char* TextSelection::GetOriginal (int& len) {
    len = text->Length();
    return text->Text();
}

// ShapedBy returns true if the TextSelection intersects a box around
// the given point.

boolean TextSelection::ShapedBy (Coord px, Coord py, float maxdist) {
    int slop = round(maxdist / 2);
    BoxObj pickpoint(px - slop, py - slop, px + slop, py + slop);
    return (LastGraphicIntersecting(pickpoint) != nil);
}

// draw readjusts the spacing between lines of text for a new font if
// necessary and sets fillbg false and pattern solid to draw the text
// like the printer will.

void TextSelection::draw (Canvas* c, Graphic* gs) {
    ReadjustSpacing(gs->GetFont());
    boolean fillbg = gs->BgFilled();
    PPattern* pattern = gs->GetPattern();

    gs->SetPattern(psolid);
    gs->FillBg(false);
    Selection::draw(c, gs);
    gs->FillBg(fillbg);
    gs->SetPattern(pattern);
}

// drawClipped readjusts the spacing between lines of text for a new
// font if necessary and sets fillbg false and pattern solid to draw
// the text like the printer will.

void TextSelection::drawClipped (Canvas* c, Coord l, Coord b, Coord r, Coord t,
Graphic* gs) {
    ReadjustSpacing(gs->GetFont());
    boolean fillbg = gs->BgFilled();
    PPattern* pattern = gs->GetPattern();

    gs->SetPattern(psolid);
    gs->FillBg(false);
    Selection::drawClipped(c, l, b, r, t, gs);
    gs->FillBg(fillbg);
    gs->SetPattern(pattern);
}

// ReadjustSpacing recalculates the spacing between lines of text.

void TextSelection::ReadjustSpacing (PFont* font) {
    if (font != nil) {
	int newHt = ((IFont*) font)->GetLineHt(); 
	if (lineHt != newHt) {
	    lineHt = newHt;
	    int vertoffset = -lineHt + 1;
	    for (First(); !AtEnd(); Next()) {
		Graphic* label = GetCurrent();
		label->SetTransformer(nil);
		label->Translate(0, vertoffset);
		vertoffset -= lineHt;
	    }
	}
    }
}

// ReadTextData reads and returns the text contained in the PostScript
// representation of the TextSelection.

const char* TextSelection::ReadTextData (istream& from, int& len) {
    TextBuffer stext(sbuf, 0, SBUFSIZE);
    char nl = '\n';

    if (versionnumber >= NONREDUNDANTVERSION) {
	Skip(from);
	int dot = 0;
	char c = ' ';
	while (from >> c && c != ']') {
	    while (c != '(' && from.get(c)) {
	    }
	    while (from.get(c) && c != ')') {
		if (c == '\\') {
		    from.get(c);
		}
		stext.Insert(dot++, &c, 1);
	    }
	    stext.Insert(dot++, &nl, 1);
	}
	stext.Delete(--dot, 1); // buffer must not terminate in '\n'
    } else {
	int dot = 0;
	while (from >> buf && strcmp(buf, startdata) == 0) {
	    char blank;
	    from.get(blank);
	    from.get(buf, BUFSIZE - 1);
	    int buflen = strlen(buf) + 1;
	    buf[buflen - 1] = '\n';
	    stext.Insert(dot, buf, buflen);
	    dot += buflen;
	}
	stext.Delete(--dot, 1); // buffer must not terminate in '\n'
    }

    len = stext.Length();
    return stext.Text();
}

// WriteData writes the TextSelection's data and Postscript code to
// draw it.

void TextSelection::WriteData (ostream& to) {
    to << "Begin " << startdata << " Text\n";
    WriteTextGS(to);
    to << startdata << "\n";
    to << "[\n";

    int lines = text->Height();
    for (int i = 0; i < lines; ++i) {
	int bol = text->LineIndex(i);
	int eol = text->EndOfLine(bol);
	const char* string = Filter(text->Text(bol, eol), eol - bol);
	to << "(" << string << ")\n";
    }

    to << "] Text\n";
    to << "End\n\n";
}

// Filter escapes embedded special characters that would cause syntax
// errors in a Postscript string.

const char* TextSelection::Filter (const char* string, int len) {
    TextBuffer stext(sbuf, 0, SBUFSIZE);
    char esc = '\\';
    char nul = '\0';

    int dot = 0;
    for (int i = 0; i < len; i++) {
	switch (*string) {
	case '(':
	case ')':
	case '\\':
	    stext.Insert(dot++, &esc, 1);
	    // fall through
	default:
	    stext.Insert(dot++, string++, 1);
	}
    }
    stext.Insert(dot++, &nul, 1);

    return stext.Text();
}
