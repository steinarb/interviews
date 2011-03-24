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

// $Header: textedit.h,v 1.9 89/10/09 14:50:01 linton Exp $
// declares class TextEdit.

#ifndef textedit_h
#define textedit_h

#include <InterViews/defs.h>

// Declare imported types.

class Canvas;
class Event;
class Painter;
class TextDisplay;
class TextBuffer;

// A TextEdit edits an array of lines of text.

class TextEdit {
public:

    TextEdit(const char* = 0, int = 0);
    ~TextEdit();

    const char* GetText(int&);

    void Redraw(Painter*, Canvas*, int lineHt, boolean redraw);
    void Grasp(Event&);
    boolean Editing(Event&);
    void Bounds(Coord& xmin, Coord& ymin, Coord& xmax, Coord& ymax);

protected:

    boolean HandleKey(char);

    void InsertCharacter(char);
    void DeleteCharacter(int);

    void InsertText(const char*, int);
    void DeleteText(int);

    void DeleteRestOfLine();
    void DeleteSelection();

    void BackwardCharacter(int = 1),    ForwardCharacter(int = 1);
    void BackwardLine(int = 1),         ForwardLine(int = 1);

    void BeginningOfLine(),             EndOfLine();

    void Select(int dot);
    void SelectMore(int mark);
    void SelectLine();
    void SelectWord();
    void Select(int dot, int mark);

    boolean Contains(Coord, Coord);
    int Locate(Coord, Coord);

protected:

    boolean selecting;
    int dot, mark;
    TextBuffer* text;
    TextDisplay* display;
    Painter* output;

};

#endif
