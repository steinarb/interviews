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

// $Header: page.h,v 1.5 90/01/25 16:27:26 interran Exp $
// declares class Page.

#ifndef page_h
#define page_h

#include <InterViews/Graphic/picture.h>

// Declare imported types.

class PictSelection;

// A Page draws a grid, picture, and boundary.  It can constrain
// points to lie only on the grid.

static const int GRID_DEFAULTSPACING = 8;

class Page : public Picture {
public:

    Page(double w, double h, double b, Graphic* = nil);
    ~Page();

    Color* GetBackgroundColor();
    boolean GetGridGravity();
    double GetGridSpacing(boolean = false);
    boolean GetGridVisibility();
    PictSelection* GetPicture();

    void SetGridGravity(boolean);
    void SetGridSpacing(double, boolean = false);
    void SetGridVisibility(boolean);
    void SetPicture(PictSelection*);

    void Center(float, float, float);
    void Constrain(Coord&, Coord&);
    void ToggleOrientation();

protected:

    void getExtent(float&, float&, float&, float&, float&, Graphic*);

    void draw(Canvas*, Graphic*);
    void drawGrid(Canvas*, Graphic*);
    void drawBoundary(Canvas*, Graphic*);

    void drawClipped(Canvas*, Coord, Coord, Coord, Coord, Graphic*);
    void drawGridClipped(Canvas*, Coord, Coord, Coord, Coord, Graphic*);
    void drawPictureClipped(Canvas*, Coord, Coord, Coord, Coord, Graphic*);
    void drawBoundaryClipped(Canvas*, Coord, Coord, Coord, Coord, Graphic*);

    int DefinePoints(Coord, Coord, Coord, Coord, Graphic*);

    double pagewidth;		// stores exact width of page
    double pageheight;		// stores exact height of page
    int pgwidth;		// stores integral width of page
    int pgheight;		// stores integral height of page
    PBrush* border;		// stores brush for drawing boundary
    boolean gravity;		// will constrain points to grid if true
    boolean visibility;		// will draw grid points if true
    double spacing_pixels;	// stores spacing in units of pixels
    double spacing_points;	// stores spacing in units of printers' points
    Coord* x, *y;		// stores grid points
    Graphic* grid_gs;		// stores attributes for drawing grid/boundary
    Transformer* grid_tt;	// stores matrix for drawing grid/boundary
    PictSelection* picture;	// stores picture

};

// Define access functions to get and set members' values.

inline boolean Page::GetGridGravity () {
    return gravity;
}

inline double Page::GetGridSpacing (boolean in_pixels) {
    return in_pixels ? spacing_pixels : spacing_points;
}

inline boolean Page::GetGridVisibility () {
    return visibility;
}

inline PictSelection* Page::GetPicture () {
    return picture;
}

inline void Page::SetGridGravity (boolean g) {
    gravity = g;
}

inline void Page::SetGridVisibility (boolean v) {
    visibility = v;
}

#endif
