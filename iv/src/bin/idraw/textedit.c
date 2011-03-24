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

// $Header: textedit.c,v 1.16 89/10/25 18:09:40 interran Exp $
// implements class TextEdit.

#include "textedit.h"
#include <InterViews/canvas.h>
#include <InterViews/event.h>
#include <InterViews/font.h>
#include <InterViews/painter.h>
#include <InterViews/textbuffer.h>
#include <InterViews/textdisplay.h>
#include <InterViews/transformer.h>
#include <bstring.h>
#include <ctype.h>

// Beware: only one instance of TextEdit can exist at any time!

static const int SBUFSIZE = 10000;
static char sbuf[SBUFSIZE];

TextEdit::TextEdit (const char* sample, int samplen) {
    selecting = false;
    dot = mark = 0;
    text = new TextBuffer(sbuf, 0, SBUFSIZE);
    display = new TextDisplay(true);
    output = nil;

    text->Insert(0, sample, samplen);
    int lines = text->Height();
    for (int i = 0; i < lines; ++i) {
        int bol = text->LineIndex(i);
        int eol = text->EndOfLine(bol);
        display->ReplaceText(i, text->Text(bol, eol), eol - bol);
    }
}

TextEdit::~TextEdit () { 
    delete text;
    delete display;
}

const char* TextEdit::GetText (int& size) { 
    size = text->Length();
    return text->Text();
}

void TextEdit::Redraw (
    Painter* output, Canvas* canvas, int lineHt, boolean redraw
) {
    TextEdit::output = output;
    display->Draw(output, canvas);

    display->LineHeight(lineHt);
    Coord l = 0;
    Coord b = 0 - display->Height();
    Coord r = 0 + display->Width();
    Coord t = 0;
    display->Resize(l, b, r, t);
    if (redraw) {
	display->Redraw(l, b, r, t);
    }
}

void TextEdit::Grasp (Event& e) {
    Select(Locate(e.x, e.y));
    selecting = true;
    e.eventType = MotionEvent;
}

boolean TextEdit::Editing (Event& e) {
    boolean editing = true;

    if (e.eventType == KeyEvent && e.len > 0) {
        editing = HandleKey(e.keystring[0]);
    } else if (e.eventType == MotionEvent && selecting) {
        SelectMore(Locate(e.x, e.y));
    } else if (e.eventType == DownEvent) {
        if (e.shift) {
            SelectMore(Locate(e.x, e.y));
            selecting = true;
        } else if (Contains(e.x, e.y)) {
            Select(Locate(e.x, e.y));
            selecting = true;
        } else {
            editing = false;
        }
    } else if (e.eventType == UpEvent) {
        selecting = false;
    }

    if (!editing) {
	Select(dot);
        display->CaretStyle(NoCaret);
    }
    return editing;
}

void TextEdit::Bounds (Coord& xmin, Coord& ymin, Coord& xmax, Coord& ymax) {
    Transformer* t = output->GetTransformer();

    display->Bounds(xmin, ymin, xmax, ymax);
    if (t != nil) {
        t->TransformRect(xmin, ymin, xmax, ymax);
    }
}

boolean TextEdit::HandleKey (char c) {
    boolean editing = true;

    switch (c) {
        case '\001' /* ^A  */:  BeginningOfLine(); break;
        case '\005' /* ^E  */:  EndOfLine(); break;
        case '\006' /* ^F  */:  ForwardCharacter(1); break;
        case '\002' /* ^B  */:  BackwardCharacter(1); break;
        case '\016' /* ^N  */:  ForwardLine(1); break;
        case '\020' /* ^P  */:  BackwardLine(1); break;
        case '\013' /* ^K  */:  DeleteRestOfLine(); break;
        case '\004' /* ^D  */:  DeleteCharacter(1); break;
        case '\010' /* ^H  */:  DeleteCharacter(-1); break;
        case '\177' /* DEL */:  DeleteCharacter(-1); break;
        case '\011' /* TAB */:  InsertCharacter(' '); break;
        case '\027' /* ^W  */:  SelectWord(); break;
        case '\025' /* ^U  */:  SelectLine(); break;
        case '\015' /* RET */:  InsertCharacter('\n'); break;
        case '\033' /* ESC */:  editing = false; break;
        default:
            if (!iscntrl(c)) {
                InsertCharacter(c);
            }
            break;
    }
    return editing;
}

void TextEdit::InsertCharacter (char c) {
    DeleteSelection();
    InsertText(&c, 1);
}

void TextEdit::DeleteCharacter (int count) {
    if (dot != mark) {
        DeleteSelection();
    } else {
        DeleteText(count);
    }
}

void TextEdit::InsertText (const char* s, int count) {
    count = text->Insert(dot, s, count);
    int sline = text->LineNumber(dot);
    int fline = text->LineNumber(dot + count);
    if (sline == fline) {
        int offset = text->LineOffset(dot);
        display->InsertText(sline, offset, text->Text(dot), count);
    } else {
        display->InsertLinesAfter(sline, fline-sline);
        for (int i = sline; i <= fline; ++i) {
            int bol = text->BeginningOfLine(text->LineIndex(i));
            int eol = text->EndOfLine(bol);
            display->ReplaceText(i, text->Text(bol, eol), eol-bol);
        }
    }
    Select(dot + count);
}

void TextEdit::DeleteText (int count) {
    int d = dot;
    int c = count;
    while (c > 0) {
        d = text->NextCharacter(d);
        --c;
    }
    while (c < 0) {
        dot = text->PreviousCharacter(dot);
        ++c;
    }
    count = d - dot;
    int sline = text->LineNumber(dot);
    int fline = text->LineNumber(d);
    text->Delete(dot, count);
    if (sline == fline) {
        int offset = text->LineOffset(dot);
        display->DeleteText(sline, offset, count);
    } else {
        int bol = text->BeginningOfLine(dot);
        int eol = text->EndOfLine(dot);
        display->DeleteLinesAfter(sline, fline-sline);
        display->ReplaceText(sline, text->Text(bol, eol), eol-bol);
    }
    Select(dot);
}

void TextEdit::DeleteRestOfLine () {
    if (dot == mark) {
	int bol = text->BeginningOfLine(dot);
	if (dot == bol) {
	    Select(dot, text->BeginningOfNextLine(dot));
	} else {
	    Select(dot, text->EndOfLine(dot));
	}
    }
    DeleteSelection();
}

void TextEdit::DeleteSelection () {
    if (dot != mark) {
        DeleteText(mark - dot);
    }
}

void TextEdit::BeginningOfLine () {
    if (dot != mark) {
        Select(min(mark, dot));
    } else {
        Select(text->BeginningOfLine(dot));
    }
}

void TextEdit::EndOfLine () {
    if (dot != mark) {
        Select(max(mark, dot));
    } else {
        Select(text->EndOfLine(dot));
    }
}

void TextEdit::ForwardCharacter (int count) {
    if (dot != mark) {
        Select(max(mark, dot));
    } else {
        int d = dot;
        while (count > 0) {
            d = text->NextCharacter(d);
            --count;
        }
        Select(d);
    }
}

void TextEdit::BackwardCharacter (int count) {
    if (dot != mark) {
        Select(min(mark, dot));
    } else {
        int d = dot;
        while (count > 0) {
            d = text->PreviousCharacter(d);
            --count;
        }
        Select(d);
    }
}

void TextEdit::ForwardLine (int count) {
    if (dot != mark) {
        Select(max(mark, dot));
    } else {
        int d = dot;
        while (count > 0) {
            d = text->BeginningOfNextLine(d);
            --count;
        }
        Select(d);
    }
}

void TextEdit::BackwardLine (int count) {
    if (dot != mark) {
        Select(min(mark, dot));
    } else {
        int d = dot;
        while (count > 0) {
            d = text->BeginningOfLine(text->EndOfPreviousLine(d));
            --count;
        }
        Select(d);
    }
}

void TextEdit::Select (int d) {
    Select(d, d);
}

void TextEdit::SelectMore (int m) {
    Select(dot, m);
}

void TextEdit::SelectLine () {
    Select(text->BeginningOfLine(dot), text->BeginningOfNextLine(dot));
}

void TextEdit::SelectWord () {
    int left = min(mark, dot);
    int right = max(mark, dot);
    Select(text->BeginningOfWord(text->PreviousCharacter(left)), right);
}

void TextEdit::Select (int d, int m) {
    int oldl = min(dot, mark);
    int oldr = max(dot, mark);
    int newl = min(d, m);
    int newr = max(d, m);
    if (oldl == oldr && newl != newr) {
        display->CaretStyle(NoCaret);
    }
    if (newr < oldl || newl > oldr) {
        if (oldr > oldl) {
            display->RemoveStyle(
                text->LineNumber(oldl), text->LineOffset(oldl),
                text->LineNumber(oldr-1), text->LineOffset(oldr-1),
                Reversed
            );
        }
        if (newr > newl) {
            display->AddStyle(
                text->LineNumber(newl), text->LineOffset(newl),
                text->LineNumber(newr-1), text->LineOffset(newr-1),
                Reversed
            );
        }
    } else {
        if (newl < oldl) {
            display->AddStyle(
                text->LineNumber(newl), text->LineOffset(newl),
                text->LineNumber(oldl-1), text->LineOffset(oldl-1),
                Reversed
            );
        } else if (newl > oldl) {
            display->RemoveStyle(
                text->LineNumber(oldl), text->LineOffset(oldl),
                text->LineNumber(newl-1), text->LineOffset(newl-1),
                Reversed
            );
        }
        if (newr > oldr) {
            display->AddStyle(
                text->LineNumber(oldr), text->LineOffset(oldr),
                text->LineNumber(newr-1), text->LineOffset(newr-1),
                Reversed
            );
        } else if (newr < oldr) {
            display->RemoveStyle(
                text->LineNumber(newr), text->LineOffset(newr),
                text->LineNumber(oldr-1), text->LineOffset(oldr-1),
                Reversed
            );
        }
    }
    if (oldl != oldr && newl == newr) {
        display->CaretStyle(BarCaret);
    }
    if (newl == newr) {
        display->Caret(text->LineNumber(newl), text->LineOffset(newl));
    }
    dot = d;
    mark = m;
}

boolean TextEdit::Contains (Coord x, Coord y) {
    Transformer tr(output->GetTransformer());
    tr.InvTransform(x, y);
    int line = display->LineNumber(y);
    int index = display->LineIndex(line, x);

    return
        x >= display->Left(line, text->BeginningOfLine(index)) &&
        x <= display->Right(line, text->EndOfLine(index)) &&
        y >= display->Base(line) &&
        y <= display->Top(line);
}

int TextEdit::Locate (Coord x, Coord y) {
    Transformer tr(output->GetTransformer());
    tr.InvTransform(x, y);
    int line = display->LineNumber(y);
    int index = display->LineIndex(line, x);
    int l = text->LineIndex(line);
    int i = 0;
    while (i < index) {
        l = text->NextCharacter(l);
        i += 1;
    }
    return l;
}
