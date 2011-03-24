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
 * StringBrowser - a Mac minifinder-like object for perusing and choosing 
 * from list of strings.
 */

#ifndef strbrowser_h
#define strbrowser_h

#include <InterViews/interactor.h>

static const char* SBDone = "\r\t\007\033";

static const char SBFirstString = 'g';
static const char SBLastString = 'G';
static const char SBSelectAll = 'a';
static const char SBUnselectAll = '\177';
static const char SBUnselectAllAlt = '\010';
static const char SBSelectPreviousString = 'p';
static const char SBSelectNextString = 'n';
static const char SBSelectTopString = '<';
static const char SBSelectBottomString = '>';
static const char SBScrollDown = 'j';
static const char SBScrollUp = 'k';
static const char SBPageDown = ' ';
static const char SBPageUp = 'b';
static const char SBHalfPageDown = 'd';
static const char SBHalfPageUp = 'u';

class ButtonState;
class TextDisplay;

class StringBrowser : public Interactor {
public:
    StringBrowser(
        ButtonState*, int rows, int cols, 
        boolean uniqueSel = true, int highlight = Reversed,
        const char* done = SBDone
    );
    StringBrowser(
        const char* name, ButtonState*, int, int,
        boolean = true, int = Reversed, const char* = SBDone
    );
    virtual ~StringBrowser();

    void Browse();
    void Insert(const char*, int index);
    void Append(const char*);
    void Remove(int index);

    int Index(const char*);
    char* String(int);
    int Count();
    void Clear();

    void Select(int index);
    void SelectAll();
    void Unselect(int index);
    void UnselectAll();
    int Selection(int selindex = 0);
    int SelectionIndex(int index);
    int Selections();
    boolean Selected(int index);

    virtual void Handle(Event&);
    virtual void Adjust(Perspective&);
protected:
    void Select(int dot, int mark);
    void Unselect(int dot, int mark);
    void ScrollBy(int, int);
    void ScrollBy(int lines);
    void ScrollTo(int, int);
    void ScrollTo(int index);
    void ScrollToView(Coord, Coord);
    void GrabScroll(Event&);
    void RateScroll(Event&);

    int Locate(Coord, Coord);
    boolean DoubleClicked(Event&);

    virtual boolean HandleChar(char);
    virtual boolean LeftButtonDown(Event&);

    virtual void Reconfig();
    virtual void Redraw(Coord, Coord, Coord, Coord);
    virtual void Resize();
protected:
    int rows;
    int columns;
    boolean uniqueSel;
    boolean singleClick;
    int clickDelay;
    int highlight;
    ButtonState* subject;
    const char* done;
private:
    void Init(ButtonState*, int, int, boolean, int, const char*);
    void InitTextDisplay();
    void UpdateSelection(int dot, int mark, int style);
    void Note(Event&);

    boolean HandleDownEvent(Event&);
    boolean HandleKeyEvent(Event&);
private:
    char** strbuf;
    int strbufsize;
    int strcount;
    char** selbuf;
    int selbufsize;
    int selcount;

    TextDisplay* display;
    int lineheight;
    unsigned long lasttime;
    Coord lastx, lasty;
    int lastdot, lastmark;
    boolean firstResize;
};

inline void StringBrowser::Append (const char* s) { Insert(s, strcount); }
inline int StringBrowser::Count () { return strcount; }
inline int StringBrowser::Selections () { return selcount; }
inline boolean StringBrowser::Selected (int i) {return SelectionIndex(i) >= 0;}

#endif
