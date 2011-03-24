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

// $Header: state.h,v 1.11 90/01/25 16:27:29 interran Exp $
// declares class State.

#ifndef state_h
#define state_h

#include <InterViews/defs.h>

// Declare imported types.

class Graphic;
class IBrush;
class IColor;
class IFont;
class IPattern;
class Interactor;
class InteractorList;
class MapIBrush;
class MapIColor;
class MapIFont;
class MapIPattern;
class Page;
class Transformer;

// A State stores state information about the user's drawing and paint
// attributes to be used when creating new Selections.

enum ModifStatus {
    ReadOnly,			// no modifications to drawing allowed
    Unmodified,			// no modifications have been made yet
    Modified			// at least one modification has been made
};

class State {
public:

    State(Interactor*, Page*);
    ~State();

    void Constrain(Coord&, Coord&);
    void ToggleOrientation();

    IBrush* GetBrush();
    IColor* GetFgColor();
    IColor* GetBgColor();
    const char* GetDrawingName();
    boolean GetFillBg();
    IFont* GetFont();
    Graphic* GetGraphicGS();
    boolean GetGridGravity();
    double GetGridSpacing(boolean = false);
    boolean GetGridVisibility();
    int GetLineHt();
    float GetMagnif();
    MapIBrush* GetMapIBrush();
    MapIColor* GetMapIFgColor();
    MapIColor* GetMapIBgColor();
    MapIFont* GetMapIFont();
    MapIPattern* GetMapIPattern();
    ModifStatus GetModifStatus();
    IPattern* GetPattern();
    Graphic* GetTextGS();
    Painter* GetTextPainter();

    void SetBrush(IBrush*);
    void SetFgColor(IColor*);
    void SetBgColor(IColor*);
    void SetDrawingName(const char*);
    void SetFillBg(boolean);
    void SetFont(IFont*);
    void SetGraphicT(Transformer&);
    void SetGridGravity(boolean);
    void SetGridSpacing(double, boolean = false);
    void SetGridVisibility(boolean);
    void SetMagnif(float);
    void SetModifStatus(ModifStatus);
    void SetPattern(IPattern*);
    void SetTextGS(Coord, Coord, Painter*);
    void SetTextGS(Graphic*, Painter*);

    void Attach(Interactor*);
    void Detach(Interactor*);
    void UpdateViews();

protected:

    char* drawingname;		// stores drawing's filename
    Graphic* graphicstate;	// stores all attributes for creating graphics
    Transformer* graphicstate_t;// stores matrix for creating graphics
    boolean gridding;		// stores true if grid will constrain points
    float magnif;		// stores drawing view's magnification factor
    MapIBrush* mapibrush;	// stores list of possible brushes
    MapIColor* mapifgcolor;	// stores list of possible fg colors
    MapIColor* mapibgcolor;	// stores list of possible bg colors
    MapIFont* mapifont;		// stores list of possible fonts
    MapIPattern* mapipattern;	// stores list of possible patterns
    ModifStatus modifstatus;	// stores drawing's modification status
    Page* page;			// stores all grid attributes
    Graphic* textgs;		// stores all attributes for creating text
    Transformer* textgs_t;	// stores matrix for creating text
    Painter* textpainter;	// stores all attributes for editing text
    Transformer* textpainter_t;	// stores matrix for editing text
    float textx;		// stores last place text was being edited
    float texty;		// stores last place text was being edited
    int lineHt;			// stores spacing between lines of text
    InteractorList* viewlist;	// stores list of views to notify

};

#endif
