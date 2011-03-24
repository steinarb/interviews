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
 * Rubberbanding curves.
 */

#include <InterViews/painter.h>
#include <InterViews/rubcurve.h>
#include <bstring.h>
#include <math.h>

RubberEllipse::RubberEllipse (
    Painter* p, Canvas* c, Coord cx, Coord cy, Coord rx, Coord ry, 
    Coord offx, Coord offy
) : (p, c, offx, offy) {
    centerx = cx;
    centery = cy;
    radiusx = rx;
    radiusy = ry;    
    trackx = rx;
    tracky = ry;
}

void RubberEllipse::GetOriginal (Coord& cx, Coord& cy, Coord& rx, Coord& ry) {
    cx = centerx;
    cy = centery;
    rx = radiusx;
    ry = radiusy;
}

void RubberEllipse::GetCurrent (Coord& cx, Coord& cy, Coord& rx, Coord& ry) {
    cx = centerx;
    cy = centery;
    rx = trackx;
    ry = tracky;
}

void RubberEllipse::OriginalRadii (int& xr, int& yr) {
    xr = abs(radiusx - centerx);
    yr = abs(radiusy - centery);
}

void RubberEllipse::CurrentRadii (int& xr, int& yr) {
    xr = abs(trackx - centerx);
    yr = abs(tracky - centery);
}

void RubberEllipse::Draw () {
    Coord cx, cy, rx, ry, xr, yr;

    if (!drawn) {
	GetCurrent(cx, cy, rx, ry);
	CurrentRadii(xr, yr);
	output->Ellipse(canvas, cx+offx, cy+offy, xr, yr);
	drawn = true;
    }
}

/**************************************************************************/

SlidingEllipse::SlidingEllipse(
    Painter* p, Canvas* c, Coord cx, Coord cy, Coord xr, Coord yr,
    Coord rfx, Coord rfy, Coord offx, Coord offy
) : (p, c, cx, cy, xr, yr, offx, offy) {
    refx = trackx = rfx;
    refy = tracky = rfy;
}

void SlidingEllipse::GetCurrent (Coord& cx, Coord& cy, Coord& xr, Coord& yr) {
    Coord dx = trackx - refx;
    Coord dy = tracky - refy;

    cx = centerx + dx;
    cy = centery + dy;
    xr = radiusx;
    yr = radiusy;
}

void SlidingEllipse::OriginalRadii (int& xr, int& yr) {
    xr = radiusx;
    yr = radiusy;
}

void SlidingEllipse::CurrentRadii (int& xr, int& yr) {
    xr = radiusx;
    yr = radiusy;
}

/*****************************************************************************/

RubberCircle::RubberCircle (
    Painter* p, Canvas* c, Coord cx, Coord cy, Coord rx, Coord ry,
    Coord offx, Coord offy
) : (p, c, cx, cy, rx, ry, offx, offy) {
    /* nothing else to do */
}

void RubberCircle::OriginalRadii (int& xr, int& yr) {
    Coord dx = radiusx - centerx;
    Coord dy = radiusy - centery;
    int radius = round(sqrt(dx*dx + dy*dy));
    xr = radius;
    yr = radius;
}

void RubberCircle::CurrentRadii (int& xr, int& yr) {
    Coord dx = trackx - centerx;
    Coord dy = tracky - centery;
    int radius = round(sqrt(dx*dx + dy*dy));
    xr = radius;
    yr = radius;
}

void RubberCircle::Draw () {
    int radius;

    if (!drawn) {
	CurrentRadii(radius, radius);
	output->Circle(canvas, centerx + offx, centery + offy, radius);
	drawn = true;
    }
}

/*****************************************************************************/

void RubberPointList::Copy (Coord* x, Coord* y, int n, Coord*& nx, Coord*& ny){
    nx = new Coord[n];
    ny = new Coord[n];
    bcopy(x, nx, n*sizeof(Coord));
    bcopy(y, ny, n*sizeof(Coord));
}

RubberPointList::RubberPointList (
    Painter* p, Canvas* c, Coord px[], Coord py[], int n,
    Coord offx, Coord offy
) : (p, c, offx, offy) {
    Copy(px, py, n, x, y);
    count = n;
}

RubberPointList::~RubberPointList () {
    delete x;
    delete y;
}

/*****************************************************************************/

RubberVertex::RubberVertex (
    Painter* p, Canvas* c, Coord px[], Coord py[], int n, int pt,
    Coord offx, Coord offy
) : (p, c, px, py, n, offx, offy) {
    rubberPt = pt;
    trackx = x[rubberPt];
    tracky = y[rubberPt];
}

void RubberVertex::GetOriginal (Coord*& px, Coord*& py, int& n, int& pt) {
    Copy(x, y, count, px, py);
    n = count;
    pt = rubberPt;
}

void RubberVertex::GetCurrent (Coord*& px, Coord*& py, int& n, int& pt) {
    Copy(x, y, count, px, py);
    n = count;
    pt = rubberPt;
    px[rubberPt] = trackx;
    py[rubberPt] = tracky;
}

void RubberVertex::DrawSplineSection (
    Painter* p, Canvas* c, Coord x[], Coord y[]
) {
    double twicex1, twicex2, p0x, p1x, p2x, p3x, tempx;
    double twicey1, twicey2, p0y, p1y, p2y, p3y, tempy;

    twicex1 = 2.0*double(x[1]);
    twicey1 = 2.0*double(y[1]);
    twicex2 = 2.0*double(x[2]);
    twicey2 = 2.0*double(y[2]);
    
    p1x = (twicex1 + double(x[2])) / 3.0;
    p1y = (twicey1 + double(y[2])) / 3.0;
    p2x = (twicex2 + double(x[1])) / 3.0;
    p2y = (twicey2 + double(y[1])) / 3.0;
    tempx = (twicex1 + double(x[0])) / 3.0;
    tempy = (twicey1 + double(y[0])) / 3.0;
    p0x = (tempx + p1x) / 2.0;
    p0y = (tempy + p1y) / 2.0;
    tempx = (twicex2 + double(x[3])) / 3.0;
    tempy = (twicey2 + double(y[3])) / 3.0;
    p3x = (tempx + p2x) / 2.0;
    p3y = (tempy + p2y) / 2.0;
    p->Curve(c,
        round(p0x)+offx, round(p0y)+offy, round(p1x)+offx, round(p1y)+offy,
	round(p2x)+offx, round(p2y)+offy, round(p3x)+offx, round(p3y)+offy
    );
}

/*****************************************************************************/

RubberHandles::RubberHandles (
    Painter* p, Canvas* c, Coord px[], Coord py[], int n, int pt, int size,
    Coord offx, Coord offy
) : (p, c, px, py, n, pt, offx, offy) {
     d = size / 2;
}

void RubberHandles::Draw () {
    register int i;

    if (x == nil || y == nil) {
        return;
    }
    if (!drawn) {
	for (i = 0; i < count; ++i) {
	    if (i == rubberPt) {
		output->FillRect(canvas,
		    trackx - d + offx, tracky - d + offy, 
		    trackx + d + offx, tracky + d + offy
		);
	    } else {
		output->FillRect(canvas,
		    x[i] - d + offx, y[i] - d + offy,
		    x[i] + d + offx, y[i] + d + offy
		);
	    }	    
	}
	drawn = true;
    }
}

void RubberHandles::Track (Coord x, Coord y) {
    if (x != trackx || y != tracky) {
        if (drawn) {
	    /* erase */
	    output->FillRect(canvas,
		trackx - d + offx, tracky - d + offy,
		trackx + d + offx, tracky + d + offy
	    );
	}
	trackx = x;
	tracky = y;
	output->FillRect(canvas,
	    trackx - d + offx, tracky - d + offy,
	    trackx + d + offx, tracky + d + offy
	);
	drawn = true;
    }
}

/*****************************************************************************/

RubberSpline::RubberSpline (
    Painter* p, Canvas* c, Coord px[], Coord py[], int n, int pt,
    Coord offx, Coord offy
) : (p, c, px, py, n, pt, offx, offy) {
    /* nothing else to do */
}

void RubberSpline::Draw () {
    register int i, j;
    Coord sx[7], sy[7];

    if (x == nil || y == nil) {
        return;
    }
    if (!drawn) {
        for (i = -3; i <= 3; ++i) {
            j = min(max(rubberPt + i, 0), count - 1);
	    if (j == rubberPt) {
	        sx[i + 3] = trackx;
		sy[i + 3] = tracky;
	    } else {
	        sx[i + 3] = x[j];
	        sy[i + 3] = y[j];
	    }
        }
        DrawSplineSection(output, canvas, sx, sy);
        DrawSplineSection(output, canvas, &sx[1], &sy[1]);
        DrawSplineSection(output, canvas, &sx[2], &sy[2]);
        DrawSplineSection(output, canvas, &sx[3], &sy[3]);
	drawn = true;
    }
}

/*****************************************************************************/

RubberClosedSpline::RubberClosedSpline (
    Painter* p, Canvas* c, Coord px[], Coord py[], int n, int pt,
    Coord offx, Coord offy
) : (p, c, px, py, n, pt, offx, offy) {
    /* nothing else to do */
}

void RubberClosedSpline::Draw () {
    register int i, j;
    Coord sx[7], sy[7];
    
    if (x == nil || y == nil) {
        return;
    }
    if (!drawn) {
	if (count > 2) {
	    for (i = -3; i <= 3; ++i) {
		j = (rubberPt + count + i) % count;
		if (j == rubberPt) {
		    sx[i + 3] = trackx;
		    sy[i + 3] = tracky;
		} else {
		    sx[i + 3] = x[j];
		    sy[i + 3] = y[j];
		}
	    }
	    DrawSplineSection(output, canvas, sx, sy);
	    DrawSplineSection(output, canvas, &sx[1], &sy[1]);
	    DrawSplineSection(output, canvas, &sx[2], &sy[2]);
	    if (count > 3) {
		DrawSplineSection(output, canvas, &sx[3], &sy[3]);
	    }
	} else {
	    i = 1 - rubberPt;
	    output->Line(canvas, x[i], y[i], trackx, tracky);
	}
	drawn = true;
    }
}

/*****************************************************************************/

SlidingPointList::SlidingPointList (
    Painter* p, Canvas* c, Coord px[], Coord py[], int n,
    Coord rfx, Coord rfy, Coord offx, Coord offy
) : (p, c, px, py, n, offx, offy) {
    refx = rfx;
    refy = rfy;
    trackx = rfx;
    tracky = rfy;
}

void SlidingPointList::GetOriginal (Coord*& px, Coord*& py, int& n) {
    register int i;
    register Coord dx = trackx - refx;
    register Coord dy = tracky - refy;

    px = new Coord[count];
    py = new Coord[count];
    n = this->count;
    for (i = 0; i < count; i++) {
        px[i] = x[i] - dx;
	py[i] = y[i] - dy;
    }
}

void SlidingPointList::GetCurrent (Coord*& px, Coord*& py, int& n) {
    Copy(x, y, count, px, py);
    n = count;
}

void SlidingPointList::Draw () {
    if (x == nil || y == nil) {
        return;
    }
    if (!drawn) {
	if (offx == 0 && offy == 0) {
	    output->MultiPoint(canvas, x, y, count);
	} else {
	    register Coord* ox = new Coord[count];
	    register Coord* oy = new Coord[count];
	    for (register int i = 0; i < count; i++) {
		ox[i] = x[i] + offx;
		oy[i] = y[i] + offy;
	    }
	    output->MultiPoint(canvas, ox, oy, count);
	    delete ox;
	    delete oy;
	}
	drawn = true;
    }
}

void SlidingPointList::Track (Coord x0, Coord y0) {
    register int i;
    register Coord dx, dy;

    if (x0 != trackx || y0 != tracky) {
        Erase();
	dx = x0 - trackx;
	dy = y0 - tracky;
	for (i = 0; i < count; i++) {
	    x[i] += dx;
	    y[i] += dy;
	}
	trackx = x0;
	tracky = y0;
	Draw();
    }
}

/*****************************************************************************/

SlidingLineList::SlidingLineList (
    Painter* p, Canvas* c, Coord px[], Coord py[], int n,
    Coord rfx, Coord rfy, Coord offx, Coord offy
) : (p, c, px, py, n, rfx, rfy, offx, offy) {
}

void SlidingLineList::Draw () {
    if (x == nil || y == nil) {
        return;
    }
    if (!drawn) {
	if (offx == 0 && offy == 0) {
	    output->MultiLine(canvas, x, y, count);
	} else {
	    register Coord* ox = new Coord[count];
	    register Coord* oy = new Coord[count];
	    for (register int i = 0; i < count; i++) {
		ox[i] = x[i] + offx;
		oy[i] = y[i] + offy;
	    }
	    output->MultiLine(canvas, ox, oy, count);
	    delete ox;
	    delete oy;
	}
	drawn = true;
    }
}

/**************************************************************************/

ScalingLineList::ScalingLineList (
    Painter* p, Canvas* c, Coord px[], Coord py[], int n,
    Coord cx, Coord cy, Coord rfx, Coord rfy, Coord offx, Coord offy
) : (p, c, px, py, n, offx, offy) {
    Copy(px, py, n, newx, newy);
    centerx = cx;
    centery = cy;
    origVal = Distance(cx, cy, rfx, rfy);
    origVal = (origVal == 0) ? 1 : origVal;
}

ScalingLineList::~ScalingLineList () {
    delete newx;
    delete newy;
}

void ScalingLineList::Update () {
    float factor = CurrentScaling();

    for (int i = 0; i < count; ++i) {
	newx[i] = round(float(x[i] - centerx)*factor) + centerx;
	newy[i] = round(float(y[i] - centery)*factor) + centery;
    }
}

void ScalingLineList::GetOriginal (Coord*& px, Coord*& py, int& n) {
    Copy(x, y, count, px, py);
    n = count;
}

void ScalingLineList::GetCurrent (Coord*& px, Coord*& py, int& n) {
    Copy(newx, newy, count, px, py);
    n = count;
}

float ScalingLineList::CurrentScaling () {
    return Distance(centerx, centery, trackx, tracky) / origVal;
}

void ScalingLineList::Track (Coord x, Coord y) {
    if (x != trackx || y != tracky) {
	Erase();
	trackx = x;
	tracky = y;
	Update();
	Draw();
    }
}    

void ScalingLineList::Draw () {
    if (!drawn) {
        output->MultiLine(canvas, newx, newy, count);
        drawn = true;
    }
}

/**************************************************************************/

RotatingLineList::RotatingLineList (
    Painter* p, Canvas* c, Coord px[], Coord py[], int n,
    Coord cx, Coord cy, Coord rfx, Coord rfy, Coord offx, Coord offy
) : (p, c, px, py, n, cx, cy, rfx, rfy, offx, offy) {
    origVal = Angle(cx, cy, rfx, rfy);
}

void RotatingLineList::Update () {
    float angle = (CurrentAngle() - origVal) * M_PI/180.0;
    float cosine = cos(angle);
    float sine = sin(angle);
    float tx, ty;
    
    for (int i = 0; i < count; ++i) {
	tx = float(x[i] - centerx);
	ty = float(y[i] - centery);
	newx[i] = round(cosine*tx - sine*ty) + centerx;
	newy[i] = round(sine*tx + cosine*ty) + centery;
    }
}

float RotatingLineList::CurrentAngle () {
    return Angle(centerx, centery, trackx, tracky);
};
