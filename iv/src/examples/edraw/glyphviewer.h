/*
 * GlyphViewer
 */

#ifndef glyphviewer_h
#define glyphviewer_h

#include <InterViews/event.h>
#include <InterViews/monoglyph.h>
#include "figureview.h"

class Canvas;
class Color;
class Cursor;
class GlyphGrabber;
class PaceMaker;
class RootGraphic;

class GlyphViewer : public MonoGlyph {
public:
    GlyphViewer(float w = -1.0, float h = -1.0);
    virtual ~GlyphViewer();

    virtual void request(Requisition&) const;
    virtual void allocate(Canvas*, const Allocation&, Extension&);
    virtual void draw(Canvas*, const Allocation&) const;
    virtual void pick(Canvas*, const Allocation&, int depth, Hit&);

    virtual Tool& tool();

    void set_policy(unsigned int);
    unsigned int get_policy();

    RootView* getroot();
    void setrootgr(Graphic*);

    const Allocation& allocation() const;
    Canvas* canvas() const;
protected:
    void initshape();
    void initgraphic();
protected:
    float _width, _height;
    Allocation _a;

    RootView* _root;
    Tool _tool;
    GlyphGrabber* _grabber;
    Canvas* _canvas;
    Graphic* _master;
    unsigned int _policy;

    const Color* _bg;
};

inline const Allocation& GlyphViewer::allocation () const { return _a; }
inline Canvas* GlyphViewer::canvas () const { return _canvas; }

#endif
