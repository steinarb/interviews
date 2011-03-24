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

// $Header: page.c,v 1.9 90/01/25 16:27:23 interran Exp $
// implements class Page.

#include "ipaint.h"
#include "page.h"
#include "slpict.h"
#include <InterViews/Graphic/lines.h>
#include <InterViews/Graphic/polygons.h>
#include <InterViews/Graphic/util.h>
#include <InterViews/canvas.h>
#include <InterViews/painter.h>
#include <InterViews/transformer.h>

// Page starts out with gravity disabled, spacing set to the default
// value, visibility disabled, and an empty picture.

Page::Page (double w, double h, double b, Graphic* gs) : (gs) {
    pagewidth = w;
    pageheight = h;
    pgwidth = round(w);
    pgheight = round(h);
    border = new PBrush(0xffff, round(b));
    gravity = false;
    spacing_pixels = 0.0;
    spacing_points = 0.0;
    visibility = false;
    x = y = nil;
    SetGridSpacing(GRID_DEFAULTSPACING);
    grid_gs = new FullGraphic;
    grid_tt = new Transformer;
    grid_gs->SetBrush(psingle);
    grid_gs->SetColors(pblack, pwhite);
    grid_gs->SetPattern(psolid);
    grid_gs->SetTransformer(grid_tt);
    grid_gs->FillBg(true);
    picture = nil;
    SetPicture(nil);
    if (GetTransformer() == nil) {
	SetTransformer(new Transformer);
    }
}

Page::~Page () {
    delete border;
    delete x; 
    delete y;
    delete grid_gs;
}

// GetBackgroundColor gets the background color for DrawingView.

Color* Page::GetBackgroundColor () {
    PColor* bg = grid_gs->GetBgColor();
    return *bg;
}

// SetPicture replaces the old picture with a new picture (creating an
// empty one if necessary) and deletes the old picture.

void Page::SetPicture (PictSelection* newpic) {
    if (newpic == nil) {
	newpic = new PictSelection;
    }
    if (picture != nil) {
	Remove(picture);
	delete picture;
    }
    picture = newpic;
    Append(picture);
}

// SetGridSpacing changes the spacing between the grid points, which
// also requires reallocating the number of points in the grid.

void Page::SetGridSpacing (double p, boolean in_pixels) {
    double old_spacing_pixels = spacing_pixels;

    if (in_pixels) {
	spacing_points = p / points;
	spacing_pixels = p;
    } else {
	spacing_points = p;
	spacing_pixels = p * points;
    }
    if (spacing_pixels != old_spacing_pixels) {
	delete x;
	delete y;
	int x_count = int(pagewidth/spacing_pixels) + 1;
	int y_count = int(pageheight/spacing_pixels) + 1;
	int count = x_count * y_count;
	x = new Coord[count];
	y = new Coord[count];
    }
}

// Center replaces the page's matrix with a freshly generated matrix
// to center the page in the window and get rid of accumulated
// roundoff errors.

void Page::Center (float mag, float wincx, float wincy) {
    Transformer* t = GetTransformer();
    float cx, cy;

    if (t->Rotated90()) {
	*t = identity;
	ToggleOrientation();
    } else {
	*t = identity;
    }
    GetCenter(cx, cy);
    Scale(mag, mag, cx, cy);
    Translate(wincx - cx, wincy - cy);
}

// Constrain replaces the given point with the closest grid point if
// gravity has been enabled.

void Page::Constrain (Coord& x, Coord& y) {
    if (gravity) {
	float x0, y0;
	grid_tt->InvTransform(float(x), float(y), x0, y0);
	x0 = round(round(x0/spacing_pixels) * spacing_pixels);
	y0 = round(round(y0/spacing_pixels) * spacing_pixels);
	grid_tt->Transform(x0, y0, x0, y0);
	x = round(x0);
	y = round(y0);
    }
}

// ToggleOrientation examines the picture's matrix to see what state
// it is in and flips the matrix to the other state.

void Page::ToggleOrientation () {
    Transformer* t = GetTransformer();
    float l, b, dx, dy;

    if (t->Rotated90()) {
	t->Transform(0.0, -pagewidth, dx, dy);
	t->Transform(0.0, 0.0, l, b);
	Translate(dx - l, dy - b);
	Rotate(90.0, l, b);
    } else {
	t->Transform(0.0, 0.0, l, b);
	t->Transform(0.0, pagewidth, dx, dy);
	Rotate(-90.0, l, b);
	Translate(dx - l, dy - b);
    }
}

// getExtent returns the page's dimensions as its extent instead of
// returning the extent of its picture so idraw's panner will always
// show the page's extent, not the picture's extent.

void Page::getExtent (float& l, float& b, float& cx, float& cy,
float& tol, Graphic* gs) {
    float dummy1, dummy2;
    transformRect(0.0, 0.0, pagewidth, pageheight, l, b, dummy1, dummy2, gs);
    transform(pagewidth/2, pageheight/2, cx, cy, gs);
    tol = 0;
}

// draw draws the grid, picture, and boundary.

void Page::draw (Canvas* c, Graphic* gs) {
    concatTransformer(nil, gs->GetTransformer(), grid_tt);
    drawGrid(c, grid_gs);
    Picture::draw(c, gs);
    drawBoundary(c, grid_gs);
}

// drawGrid passes the work off to drawGridClipped.

void Page::drawGrid (Canvas* c, Graphic* gs) {
    Coord xmax = c->Width();
    Coord ymax = c->Height();
    drawGridClipped(c, 0, 0, xmax, ymax, gs);
}

// drawBoundary draws the boundary around the page.

void Page::drawBoundary (Canvas* c, Graphic* gs) {
    gs->SetBrush(border);
    update(gs);
    pRect(c, 0, 0, pgwidth, pgheight);
}

// drawClipped draws part of the grid, picture, and boundary.

void Page::drawClipped (
    Canvas* c, Coord l, Coord b, Coord r, Coord t, Graphic* gs
) {
    concatTransformer(nil, gs->GetTransformer(), grid_tt);
    drawGridClipped(c, l, b, r, t, grid_gs);
    drawPictureClipped(c, l, b, r, t, gs);
    drawBoundaryClipped(c, l, b, r, t, grid_gs);
}

// drawGridClipped grids the given area with points if visibility has
// been enabled.  It sends the points in chunks of MAXCHUNK points
// each because Xlib does not yet break up too-big requests.

void Page::drawGridClipped (Canvas* c, Coord l, Coord b, Coord r, Coord t,
Graphic* gs) {
    if (visibility) {
	gs->SetBrush(psingle);
	update(gs);
	int ntotal = DefinePoints(l, b, r, t, gs);
	int nsent = 0;
	while (nsent < ntotal) {
	    const int MAXCHUNK = 6000;
	    int nchunk = min(MAXCHUNK, ntotal - nsent);
	    pMultiPoint(c, &x[nsent], &y[nsent], nchunk);
	    nsent += nchunk;
	}
    }
}

// drawPictureClipped goes right ahead and draws the picture, letting
// it decide if its extent intersects the clipping box.

void Page::drawPictureClipped (
    Canvas* c, Coord l, Coord b, Coord r, Coord t, Graphic* gs
) {
    register RefList* i;
    Graphic* gr;
    FullGraphic gstemp;
    Transformer ttemp;
    
    gstemp.SetTransformer(&ttemp);
    for (i = refList->First(); i != refList->End(); i = i->Next()) {
	gr = getGraphic(i);
	concatGraphic(gr, gr, gs, &gstemp);
	drawClippedGraphic(gr, c, l, b, r, t, &gstemp);
    }
    gstemp.SetTransformer(nil); /* to avoid deleting ttemp explicitly */
}

// drawBoundaryClipped draws part of the boundary around the page.

void Page::drawBoundaryClipped (
    Canvas* c, Coord l, Coord b, Coord r, Coord t, Graphic* gs
) {
    BoxObj clipBox(l, b, r, t);
    
    Coord x0, y0, x1, y1, x2, y2, x3, y3;
    transform(0, 0, x0, y0, gs);
    transform(0, pgheight, x1, y1, gs);
    transform(pgwidth, 0, x2, y2, gs);
    transform(pgwidth, pgheight, x3, y3, gs);
    LineObj lLine(x0, y0, x1, y1);
    LineObj bLine(x0, y0, x2, y2);
    LineObj rLine(x2, y2, x3, y3);
    LineObj tLine(x1, y1, x3, y3);

    gs->SetBrush(border);
    update(gs);
    if (clipBox.Intersects(lLine)) {
	pLine(c, 0, 0, 0, pgheight);
    }
    if (clipBox.Intersects(bLine)) {
	pLine(c, 0, 0, pgwidth, 0);
    }
    if (clipBox.Intersects(rLine)) {
	pLine(c, pgwidth, 0, pgwidth, pgheight);
    }
    if (clipBox.Intersects(tLine)) {
	pLine(c, 0, pgheight, pgwidth, pgheight);
    }
}

// DefinePoints defines just enough points to grid the given area.

int Page::DefinePoints (Coord l, Coord b, Coord r, Coord t, Graphic* gs) {
    float x0, y0, x1, y1;
    invTransformRect(float(l),float(b),float(r),float(t), x0, y0, x1, y1, gs);
    x0 = round(x0/spacing_pixels) * spacing_pixels;
    y0 = round(y0/spacing_pixels) * spacing_pixels;
    x0 = max(0.0, x0);
    y0 = max(0.0, y0);
    x1 = min(x1, pagewidth);
    y1 = min(y1, pageheight);
    int x_count = int((x1 - x0)/spacing_pixels) + 1;
    int y_count = int((y1 - y0)/spacing_pixels) + 1;
    int count = x_count * y_count;

    for (int i = 0; i < x_count; i++) {
	Coord ix = round(x0 + i*spacing_pixels);
	for (int j = i; j < count; j += x_count) {
	    x[j] = ix;
	}
    }

    for (i = 0; i < y_count; i++) {
	Coord iy = round(y0 + i*spacing_pixels);
	for (int j = i * x_count; j < (i+1) * x_count; j++) {
	    y[j] = iy;
	}
    }

    return count;
}
