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

// $Header: drawing.h,v 1.13 89/10/09 14:47:50 linton Exp $
// declares class Drawing.

#ifndef drawing_h
#define drawing_h

#include <InterViews/defs.h>

// Declare imported types.

class CenterList;
class Graphic;
class GroupList;
class IBrush;
class IBrushList;
class IColor;
class IColorList;
class IFont;
class IFontList;
class IPattern;
class IPatternList;
class Page;
class PictSelection;
class Selection;
class SelectionList;
class State;
class Transformer;
class booleanList;

// A Drawing contains the user's picture and provides the interface
// through which editing operations modify it.

class Drawing {
public:

    Drawing(double w, double h, double b);
    ~Drawing();

    boolean GetLandscape();
    Page* GetPage();
    void GetPictureTT(Transformer&);
    SelectionList* GetSelectionList();

    boolean Writable(const char*);
    boolean Exists(const char*);
    void ClearPicture();
    boolean ReadPicture(const char*, State*);
    boolean PrintPicture(const char*, State*);
    boolean WritePicture(const char*, State*);
    SelectionList* ReadClipboard(State*);
    void WriteClipboard();

    void GetBox(Coord&, Coord&, Coord&, Coord&);
    IBrushList* GetBrush();
    CenterList* GetCenter();
    GroupList* GetChildren();
    IColorList* GetFgColor();
    IColorList* GetBgColor();
    SelectionList* GetDuplicates();
    booleanList* GetFillBg();
    IFontList* GetFont();
    int GetNumberOfGraphics();
    GroupList* GetParent();
    IPatternList* GetPattern();
    SelectionList* GetPrevs();
    SelectionList* GetSelections();

    Selection* PickSelectionIntersecting(Coord, Coord);
    Selection* PickSelectionShapedBy(Coord, Coord);
    SelectionList* PickSelectionsWithin(Coord, Coord, Coord, Coord);

    void Clear();
    void Extend(Selection*);
    void Extend(SelectionList*);
    void Grasp(Selection*);
    void Select(Selection*);
    void Select(SelectionList*);
    void SelectAll();

    void Move(float, float);
    void Scale(float, float);
    void Stretch(float, Alignment);
    void Rotate(float);
    void Align(Alignment, Alignment);
    void AlignToGrid();

    void SetBrush(IBrush*);
    void SetBrush(IBrushList*);
    void SetCenter(CenterList*);
    void SetFgColor(IColor*);
    void SetFgColor(IColorList*);
    void SetBgColor(IColor*);
    void SetBgColor(IColorList*);
    void SetFillBg(boolean);
    void SetFillBg(booleanList*);
    void SetFont(IFont*);
    void SetFont(IFontList*);
    void SetPattern(IPattern*);
    void SetPattern(IPatternList*);

    void Append();
    void Group(GroupList*);
    void InsertAfterPrev(SelectionList*);
    void Prepend();
    void Remove();
    void Replace(Selection*, Selection*);
    void Sort();
    void Ungroup(GroupList*);

protected:

    int NumberOfGraphics(PictSelection*);

    char* clipfilename;		// filename under which to store clippings
    Page* page;			// draws picture
    PictSelection* picture;	// stores picture
    SelectionList* sl;		// lists picked Selections

};

#endif
