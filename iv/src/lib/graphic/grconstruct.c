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
 * Implementation of GraphicConstruct object construction function.
 */

#include <InterViews/Graphic/base.h>
#include <InterViews/Graphic/ellipses.h>
#include <InterViews/Graphic/instance.h>
#include <InterViews/Graphic/grclasses.h>
#include <InterViews/Graphic/grconstruct.h>
#include <InterViews/Graphic/label.h>
#include <InterViews/Graphic/lines.h>
#include <InterViews/Graphic/picture.h>
#include <InterViews/Graphic/polygons.h>
#include <InterViews/Graphic/rasterrect.h>
#include <InterViews/Graphic/splines.h>
#include <InterViews/Graphic/stencil.h>

Persistent* GraphicConstruct (ClassId id) {
    switch (id) {
	case BOXOBJ:		return new BoxObj;
	case BSPLINE:		return new BSpline;
	case CIRCLE:		return new Circle;
	case CLOSEDBSPLINE:	return new ClosedBSpline;
	case ELLIPSE:		return new Ellipse;
	case FILLBSPLINE:	return new FillBSpline;
	case FILLCIRCLE:	return new FillCircle;
	case FILLELLIPSE:	return new FillEllipse;
	case FILLPOLYGON:	return new FillPolygon;
	case FILLPOLYGONOBJ:	return new FillPolygonObj;
	case FILLRECT:		return new FillRect;
	case FULL_GRAPHIC:	return new FullGraphic;
	case GRAPHIC:		return new Graphic;
	case INSTANCE:		return new Instance;
	case LABEL:		return new Label;
	case LINE:		return new Line;
	case LINEOBJ:		return new LineObj;
	case MULTILINE:		return new MultiLine;
	case MULTILINEOBJ:	return new MultiLineObj;
	case PBRUSH:		return new PBrush;
	case PCOLOR:		return new PColor;
	case PFONT:		return new PFont;
	case PICTURE:		return new Picture;
	case POINT:		return new Point;
	case POINTOBJ:		return new PointObj;
	case POLYGON:		return new Polygon;
	case PPATTERN:		return new PPattern;
        case RASTERRECT:        return new RasterRect;
	case RECT:		return new Rect;
        case STENCIL:           return new Stencil;

	default:		return nil;
    }
}
