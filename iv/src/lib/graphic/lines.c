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
 * Implementation of Points, Lines, and MultiLines, objects derived from
 * Graphic.
 */

#include <InterViews/Graphic/grclasses.h>
#include <InterViews/Graphic/lines.h>
#include <InterViews/Graphic/util.h>
#include <stdlib.h>

boolean Point::read(PFile* f) {
    return Graphic::read(f) && brush.Read(f) && f->Read(x) && f->Read(y);
}

boolean Point::write(PFile* f) {
    return Graphic::write(f) && brush.Write(f) && f->Write(x) && f->Write(y);
}

void Point::draw(Canvas* c, Graphic* gs) {
    if (gs->GetBrush()->Width() != NO_WIDTH) {
	update(gs);
	pPoint(c, x, y);
    }
}

ClassId Point::GetClassId() { return POINT; }
boolean Point::IsA(ClassId id) { return POINT == id || Graphic::IsA(id); }
Point::Point() {}

Point::Point(Coord x, Coord y, Graphic* gr) : Graphic(gr) { 
    if (gr == nil) {
	SetBrush(nil);
    } else {
	SetBrush(gr->GetBrush());
    }
    this->x = x; 
    this->y = y;
}

Graphic* Point::Copy() { return new Point(x, y, this); }

void Point::GetOriginal(Coord& x, Coord& y) {
    x = this->x;
    y = this->y;
}

void Point::getExtent(
    float& l, float& b, float& cx, float& cy, float& tol, Graphic* gs
) {
    float width;

    width = float(gs->GetBrush()->Width());
    tol = (width > 1) ? width / 2 : 0;
    transform(float(x), float(y), cx, cy, gs);
    l = cx;
    b = cy;
}

boolean Point::contains(PointObj& po, Graphic* gs) {
    PointObj pt (&po);
    invTransform(pt.x, pt.y, gs);
    return (pt.x == x) && (pt.y == y);
}

boolean Point::intersects(BoxObj& b, Graphic* gs) {
    PointObj pt (this->x, this->y);
        
    transform(pt.x, pt.y, gs);
    return b.Contains(pt);
}

void Point::SetBrush(PBrush* brush) {
    if (this->brush != Ref(brush)) {
	this->brush = Ref(brush);
	invalidateCaches();
    }
}

PBrush* Point::GetBrush() { return (PBrush*) brush(); }

boolean Line::read(PFile* f) {
    return Graphic::read(f) && brush.Read(f) &&
	f->Read(x0) && f->Read(y0) && f->Read(x1) && f->Read(y1);
}

boolean Line::write(PFile* f) {
    return Graphic::write(f) && brush.Write(f) &&
	f->Write(x0) && f->Write(y0) && f->Write(x1) && f->Write(y1);
}

void Line::draw(Canvas* c, Graphic* gs) {
    if (gs->GetBrush()->Width() != NO_WIDTH) {
	update(gs);
	pLine(c, x0, y0, x1, y1);
    }
}

ClassId Line::GetClassId() { return LINE; }
boolean Line::IsA(ClassId id) { return LINE == id || Graphic::IsA(id); }
Line::Line() {}

Line::Line(
    Coord x0, Coord y0, Coord x1, Coord y1, Graphic* gr
) : Graphic(gr) {
    if (gr == nil) {
	SetBrush(nil);
    } else {
	SetBrush(gr->GetBrush());
    }
    this->x0 = x0;
    this->y0 = y0;
    this->x1 = x1;
    this->y1 = y1;
}

Graphic* Line::Copy() { return new Line(x0, y0, x1, y1, this); }

void Line::GetOriginal(Coord& x0, Coord& y0, Coord& x1, Coord& y1) {
    x0 = this->x0;
    y0 = this->y0;
    x1 = this->x1;
    y1 = this->y1;
}    

void Line::getExtent(
    float& l, float& b, float& cx, float& cy, float& tol, Graphic* gs
) {
    float r, t, width;

    width = float(gs->GetBrush()->Width());
    tol = (width > 1) ? width / 2 : 0;
    
    transform(float(x0+x1)/2, float(y0+y1)/2, cx, cy, gs);
    transform(float(x0), float(y0), l, b, gs);
    transform(float(x1), float(y1), r, t, gs);
    l = min(l, r);
    b = min(b, t);
}

boolean Line::contains(PointObj& po, Graphic* gs) {
    LineObj l (x0, y0, x1, y1);
    PointObj pt (&po);
    invTransform(pt.x, pt.y, gs);
    return l.Contains(pt);
}

boolean Line::intersects(BoxObj& b, Graphic* gs) {
    LineObj l (this->x0, this->y0, this->x1, this->y1);
    transform(l.p1.x, l.p1.y, gs);
    transform(l.p2.x, l.p2.y, gs);
    return b.Intersects(l);
}

void Line::SetBrush(PBrush* brush) {
    if (this->brush != Ref(brush)) {
	this->brush = Ref(brush);
	invalidateCaches();
    }
    this->brush = Ref(brush);
}

PBrush* Line::GetBrush() { return (PBrush*) brush(); }
ClassId MultiLine::GetClassId() { return MULTILINE; }

boolean MultiLine::IsA(ClassId id) { 
    return MULTILINE == id || Graphic::IsA(id);
}

MultiLine::MultiLine() {
    extent = nil;
    x = y = nil;
    count = 0;
}

MultiLine::MultiLine(
    Coord* x, Coord* y, int count, Graphic* gr
) : Graphic(gr) {
    extent = nil;
    if (gr == nil) {
	SetBrush(nil);
    } else {
	SetBrush(gr->GetBrush());
    }
    this->x = new Coord[count];
    this->y = new Coord[count];
    this->count = count;
    CopyArray(x, y, count, this->x, this->y);
}

Graphic* MultiLine::Copy() { return new MultiLine(x, y, count, this); }

MultiLine::~MultiLine() {
    uncacheExtent();
    delete x; 
    delete y;
}

boolean MultiLine::operator ==(MultiLine& ml) {
    if (count == ml.count) {
        for (int i = 0; i < count; ++i) {
            if (x[i] != ml.x[i] || y[i] != ml.y[i]) {
                return false;
            }
        }
        return true;
    }
    return false;
}

boolean MultiLine::operator !=(MultiLine& ml) { return !(*this == ml); }

boolean MultiLine::read(PFile* f) {
    boolean ok = Graphic::read(f) && _patbr.Read(f) && f->Read(count);
    if (ok) {
	delete x;
	delete y;
	x = new Coord [count];
	y = new Coord [count];
	ok = f->Read(x, count) && f->Read(y, count);
    }
    return ok;
}

boolean MultiLine::write(PFile* f) {
    return Graphic::write(f) && _patbr.Write(f) && f->Write(count) && 
	f->Write(x, count) && f->Write(y, count);
}

boolean MultiLine::extentCached() { return caching && extent != nil; }

void MultiLine::uncacheExtent() { 
    delete extent; 
    extent = nil;
}

void MultiLine::draw(Canvas *c, Graphic* gs) {
    if (gs->GetBrush()->Width() != NO_WIDTH) {
	update(gs);
	pMultiLine(c, x, y, count);
    }
}

void MultiLine::cacheExtent(float l, float b, float cx, float cy, float tol) {
    if (caching) {
	uncacheExtent();
	extent = new Extent(l, b, cx, cy, tol);
    }
}

void MultiLine::getCachedExtent(
    float& l, float& b, float& cx, float& cy, float& tol
) {
    l = extent->left;
    b = extent->bottom;
    cx = extent->cx;
    cy = extent->cy;
    tol = extent->tol;
}

void MultiLine::getExtent(
    float& l, float& b, float& cx, float& cy, float& tol, Graphic* gs
) {
    int i;
    float bx0, by0, bx1, by1, tcx, tcy, width, dummy1, dummy2;

    if (extentCached()) {
	getCachedExtent(bx0, by0, tcx, tcy, tol);
	bx1 = 2*tcx - bx0;
	by1 = 2*tcy - by0;
    } else {
	width = float(gs->GetBrush()->Width());
	tol = (width > 1) ? width/2 : 0;
	bx0 = bx1 = x[0]; by0 = by1 = y[0];
	for (i = 1; i < count; ++i) {
	    bx0 = min(bx0, float(x[i]));
	    by0 = min(by0, float(y[i]));
	    bx1 = max(bx1, float(x[i]));
	    by1 = max(by1, float(y[i]));
	}
	tcx = (bx0 + bx1) / 2;
	tcy = (by0 + by1) / 2;
	cacheExtent(bx0, by0, tcx, tcy, tol);
    }
    transformRect(bx0, by0, bx1, by1, l, b, dummy1, dummy2, gs);
    transform(tcx, tcy, cx, cy, gs);
}

void MultiLine::GetOriginal(Coord*& x, Coord*& y, int& n) {
    n = count;
    x = new Coord[n];
    y = new Coord[n];
    CopyArray(this->x, this->y, count, x, y);
}

int MultiLine::GetOriginal(const Coord*& x, const Coord*& y) {
    x = this->x;
    y = this->y;
    return count;
}

boolean MultiLine::contains(PointObj& po, Graphic* gs) {
    MultiLineObj ml (x, y, count);
    PointObj pt (&po);
    BoxObj b;

    getBox(b, gs);
    if (b.Contains(po)) {
	invTransform(pt.x, pt.y, gs);
	return ml.Contains(pt);
    }
    return false;
}

boolean MultiLine::intersects(BoxObj& userb, Graphic* gs) {
    Coord* convx, *convy;
    BoxObj b;
    boolean result = false;

    getBox(b, gs);
    if (b.Intersects(userb)) {
	convx = new Coord[count];
	convy = new Coord[count];
	transformList(x, y, count, convx, convy, gs);
	MultiLineObj ml (convx, convy, count);
	result = ml.Intersects(userb);
	delete convx;
	delete convy;
    }
    return result;
}

void MultiLine::SetBrush(PBrush* brush) {
    if (br() != Ref(brush)) {
	br(Ref(brush));
	invalidateCaches();
    }
}

PBrush* MultiLine::GetBrush() { return (PBrush*) br()(); }
void MultiLine::SetPattern(PPattern* pattern) { pat(Ref(pattern)); }
PPattern* MultiLine::GetPattern() { return (PPattern*) pat()(); }

void MultiLine::pat(Ref) { }
Ref MultiLine::pat() { return (MultiLine*)nil; }
void MultiLine::br(Ref r) { _patbr = r; }
Ref MultiLine::br() { return _patbr; }
