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
 * GraphicBlock - an interactor that contains a picture.
 */

#ifndef grblock_h
#define grblock_h

#include <InterViews/interactor.h>

typedef enum Zooming { Continuous, Binary };

class Graphic;
class Perspective;

class GraphicBlock : public Interactor {
public:
    GraphicBlock(
	Sensor*, Graphic*, 
	Coord pad = 0, Alignment = Center, Zooming = Continuous
    );
    ~GraphicBlock();

    virtual void Update();
    virtual void Draw();
    virtual void Adjust(Perspective&);
    virtual void Reconfig();
    virtual void Highlight(boolean);

    Graphic* GetGraphic();
    float GetMagnification();
    void SetMagnification(float);

    virtual void Invert();          /* obsolete; use Highlight */
protected:
    virtual void Resize();
    virtual void Redraw(Coord, Coord, Coord, Coord);

    void SwapPainters();	    /* background: a hack to avoid SetColors */
    Painter* GetPainter();	    /* in menu highlighting. */
    void InvertPainter();
    void Init();

    void Normalize(Perspective&);   /* normalize units */
    void Align();		    /* align graphic */
    void Fix();			    /* keep alignment fixed during resize */
    float NearestPow2(float);	    /* convert to nearest power of 2 */
    float ScaleFactor(Perspective&);

    void UpdatePerspective();       /* recalc based on graphic's bbox */
    void GrabScroll(Event&);
    void RateScroll(Event&);

    virtual void GetGraphicBox(Coord&, Coord&, Coord&, Coord&);
    virtual void Zoom(Perspective&);
    virtual void Scroll(Perspective&);
    virtual float LimitMagnification(float);
protected:
    Graphic* graphic;
    Coord pad;
    Alignment align;
    Zooming zooming;
    Coord x0, y0;                   /* graphic offset */
    float mag;			    /* total magnification */
    Painter* alt;		    /* alternate painter for drawing */
    boolean highlighted;
};

inline Graphic* GraphicBlock::GetGraphic () { return graphic; }
inline float GraphicBlock::GetMagnification () { return mag; }

#endif
