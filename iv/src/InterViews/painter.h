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
 * Graphics interface
 */

#ifndef painter_h
#define painter_h

#include <InterViews/defs.h>
#include <InterViews/resource.h>

class Canvas;
class Color;
class PainterRep;
class Pattern;
class Brush;
class Font;
class Transformer;
class Bitmap;
class Raster;

class Painter : public Resource {
public:
    Painter();
    Painter(Painter*);
    ~Painter();

    void FillBg(boolean);
    boolean BgFilled();
    void SetColors(Color* f, Color* b);
    Color* GetFgColor();
    Color* GetBgColor();
    void SetPattern(Pattern*);
    Pattern* GetPattern();
    void SetBrush(Brush*);
    Brush* GetBrush();
    void SetFont(Font*);
    Font* GetFont();
    void SetStyle(int);
    int GetStyle();
    void SetTransformer(Transformer*);
    Transformer* GetTransformer();
    void MoveTo(int x, int y);
    void GetPosition(int& x, int& y);
    void SetOrigin(int x0, int y0);
    void GetOrigin(int& x0, int& y0);

    void Translate(float dx, float dy);
    void Scale(float x, float y);
    void Rotate(float angle);

    void Clip(Canvas*, Coord left, Coord bottom, Coord right, Coord top);
    void NoClip();
    void SetOverwrite(boolean);
    void SetPlaneMask(int);

    void Text(Canvas*, const char*);
    void Text(Canvas*, const char*, int);
    void Text(Canvas*, const char*, Coord, Coord);
    void Text(Canvas*, const char*, int, Coord, Coord);
    void Stencil(Canvas*, Coord x, Coord y, Bitmap* image, Bitmap* mask = nil);
    void RasterRect(Canvas*, Coord x, Coord y, Raster*);
    void Point(Canvas*, Coord x, Coord y);
    void MultiPoint(Canvas*, Coord x[], Coord y[], int n);
    void Line(Canvas*, Coord x1, Coord y1, Coord x2, Coord y2);
    void Rect(Canvas*, Coord x1, Coord y1, Coord x2, Coord y2);
    void FillRect(Canvas*, Coord x1, Coord y1, Coord x2, Coord y2);
    void ClearRect(Canvas*, Coord x1, Coord y1, Coord x2, Coord y2);
    void Circle(Canvas*, Coord x, Coord y, int r);
    void FillCircle(Canvas*, Coord x, Coord y, int r);
    void Ellipse(Canvas*, Coord x, Coord y, int r1, int r2);
    void FillEllipse(Canvas*, Coord x, Coord y, int r1, int r2);
    void MultiLine(Canvas*, Coord x[], Coord y[], int n);
    void Polygon(Canvas*, Coord x[], Coord y[], int n);
    void FillPolygon(Canvas*, Coord x[], Coord y[], int n);
    void BSpline(Canvas*, Coord x[], Coord y[], int n);
    void ClosedBSpline(Canvas*, Coord x[], Coord y[], int n);
    void FillBSpline(Canvas*, Coord x[], Coord y[], int n);
    void Curve(Canvas*,
	Coord x0, Coord y0, Coord x1, Coord y1,
	Coord x2, Coord y2, Coord x3, Coord y3
    );
    void CurveTo(Canvas*,
	Coord x0, Coord y0, Coord x1, Coord y1, Coord x2, Coord y2
    );
    void Copy(
	Canvas* src, Coord x1, Coord y1, Coord x2, Coord y2,
	Canvas* dst, Coord x0, Coord y0
    );
    void Read(Canvas*, void*, Coord x1, Coord y1, Coord x2, Coord y2);
    void Write(Canvas*, const void*, Coord x1, Coord y1, Coord x2, Coord y2);

    PainterRep* Rep();
protected:
    Color* foreground;
    Color* background;
    Pattern* pattern;
    Brush* br;
    Font* font;
    int style;
    Coord curx, cury;
    int xoff, yoff;
    Transformer* matrix;
private:
    friend class Rubberband;

    PainterRep* rep;

    void Init();
    void Copy(Painter*);
    void Begin_xor();
    void End_xor();
    void Map(Canvas*, Coord x, Coord y, Coord& mx, Coord& my);
    void Map(Canvas*, Coord x, Coord y, short& sx, short& sy);
    void MapList(Canvas*, Coord x[], Coord y[], int n, Coord mx[], Coord my[]);
    void MapList(Canvas*, float x[], float y[], int n, Coord mx[], Coord my[]);
    void MultiLineNoMap(Canvas* c, Coord x[], Coord y[], int n);
    void FillPolygonNoMap(Canvas* c, Coord x[], Coord y[], int n);
};

inline PainterRep* Painter::Rep () { return rep; }

#endif
