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
 * TextEditor - basic interactive editor for mulit-line text
 */

#ifndef texteditor_h
#define texteditor_h

#include <InterViews/interactor.h>

class TextDisplay;
class TextBuffer;

class TextEditor : public Interactor {
public:
    TextEditor(int rows, int cols, int tabsize, int highlight);
    TextEditor(const char* name, int r, int c, int t, int h);
    virtual ~TextEditor();

    void Edit(TextBuffer*, int index = 0);

    int Dot();
    int Mark();

    void InsertText(const char*, int);
    void DeleteText(int);
    void DeleteSelection();

    void BackwardCharacter(int = 1),    ForwardCharacter(int = 1);
    void BackwardLine(int = 1),         ForwardLine(int = 1);
    void BackwardWord(int = 1),         ForwardWord(int = 1);
    void BackwardPage(int = 1),         ForwardPage(int = 1);

    void BeginningOfLine(),             EndOfLine();
    void BeginningOfWord(),             EndOfWord();
    void BeginningOfSelection(),        EndOfSelection();
    void BeginningOfText(),             EndOfText();

    void SetScrollAlignment(Alignment);
    Alignment GetScrollAlignment();

    void ScrollToSelection(boolean always = false);
    void ScrollToView(Coord x, Coord y);
    void ScrollBy(Coord dx, Coord dy);
    void GrabScroll(Event&);
    void RateScroll(Event&);
    virtual void Adjust(Perspective&);

    void Select(int dot);
    void SelectMore(int mark);
    void SelectAll();
    void Select(int dot, int mark);

    int Locate(Coord x, Coord y);
protected:
    virtual void Reconfig();
    virtual void Redraw(Coord, Coord, Coord, Coord);
    virtual void Resize();

    TextBuffer* text;
    TextDisplay* display;
private:
    void ScrollTo(int x, int y);
    void Init(int r, int c, int t, int h);

    int dot, mark;
    int tabsize;
    int lineheight;
    int highlight;
    int shaperows;
    int shapecolumns;
    Alignment scrollalign;
};

inline int TextEditor::Dot () { return dot; }
inline int TextEditor::Mark () { return mark; }
inline void TextEditor::SetScrollAlignment (Alignment a) { scrollalign = a; }
inline Alignment TextEditor::GetScrollAlignment() { return scrollalign; }

#endif
