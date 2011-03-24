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
 * TextDisplay - basic text displaying
 */

#ifndef textdisplay_h
#define textdisplay_h

#include <InterViews/defs.h>

enum CaretStyleOptions {
    NoCaret, DefaultCaret, BarCaret, UnderscoreCaret, OutlineCaret
};

class Painter;
class Canvas;

class TextDisplay {
public:
    TextDisplay(boolean autosized = false);
    ~TextDisplay();

    void Draw(Painter*, Canvas*);
    void LineHeight(Coord height);
    void TabWidth(Coord width);

    void Scroll(int line, Coord x, Coord y);

    void Resize(Coord xmin, Coord ymin, Coord xmax, Coord ymax);
    void Bounds(Coord& xmin, Coord& ymin, Coord& xmax, Coord& ymax);
    void Redraw(Coord l, Coord b, Coord r, Coord t);

    void InsertLinesAfter(int line, int count = 1);
    void InsertLinesBefore(int line, int count = 1);
    void DeleteLinesAfter(int line, int count = 1);
    void DeleteLinesBefore(int line, int count = 1);

    void InsertText(int line, int index, const char*, int count);
    void DeleteText(int line, int index, int count);
    void ReplaceText(int line, const char*, int count);

    void Style(int line1, int index1, int line2, int index2, int style);
    void AddStyle(int line1, int index1, int line2, int index2, int style);
    void RemoveStyle(int line1, int index1, int line2, int index2, int style);

    void Caret(int line, int index);
    void CaretStyle(int);

    int LineNumber(Coord y);
    int LineIndex(int line, Coord x, boolean between = true);

    Coord Width();
    Coord Height();

    Coord Base(int line);
    Coord Top(int line);
    Coord Left(int line, int index);
    Coord Right(int line, int index);
private:
friend class TextLine;

    void Size(int, int);
    TextLine* Line(int, boolean);
    int Index(int);
    void HideCaret();
    void ShowCaret();

    Painter* painter;
    Canvas* canvas;
    boolean autosized;
    Coord xmin, xmax;
    Coord ymin, ymax;
    Coord x0, y0;
    Coord width;
    int lineheight;
    int tabwidth;
    TextLine** lines;
    int maxlines;
    int firstline;
    int lastline;
    int topline;
    int bottomline;
    int widestline;
    int caretline;
    int caretindex;
    int caretstyle;
};

#endif
