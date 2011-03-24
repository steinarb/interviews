/*
 * planar figureviews
 */

#ifndef figureview_h
#define figureview_h

#include <InterViews/action.h>
#include <InterViews/event.h>
#include <InterViews/resource.h>
#include "figure.h"
#include "globals.h"

class Color;
class GlyphViewer;
class Graphic;
class GrViewList;
class Transformer;

typedef long ViewIndex;

class ViewAction : public Action {
public:
    virtual boolean executable() const;

    Graphic* target() const;
protected:
    ViewAction(Graphic* = nil);
protected:
    Graphic* _gr;
};

class ToolState {
public:
    Event _init;
    Event _last;
    Coord _l, _b, _r, _t;
    Graphic _gs;
};

class Tool {
public:
    enum {
        nop, select, move, scale, stretch, rotate, alter, create, narrow,
        rate_scroll, grab_scroll, rate_zoom
    };

    Tool(unsigned int = Tool::nop);
    virtual ~Tool();

    virtual unsigned int tool();
    virtual void tool(unsigned int);
    
    virtual ToolState& toolstate();
    virtual void toolstate(ToolState*);
    virtual void reset();
protected:
    unsigned int _cur_tool;
    ToolState* _toolstate;
};

class GrView : public Resource{
public:
    GrView(Graphic* gr = nil);
    virtual ~GrView ();

    virtual Graphic* getgraphic() const;
    virtual void setgraphic(Graphic*);

    virtual boolean grasp(const Event&, Tool&, ViewAction*&);
    virtual boolean manipulating(const Event&, Tool&, ViewAction*&);
    virtual void effect(const Event&, Tool&, ViewAction*&);

    virtual void append(GrView*);
    virtual void prepend(GrView*);
    virtual void insert(ViewIndex, GrView*);
    virtual void remove(ViewIndex);
    virtual void remove(GrView*);

    virtual ViewIndex count() const;
    virtual GrView* child(ViewIndex) const;
    virtual void setindex(ViewIndex);
    virtual ViewIndex getindex() const;
protected:
    Graphic* _gr;
    ViewIndex _index;
};

class PolyView : public GrView {
public:
    PolyView(Graphic* = nil) ;
    virtual ~PolyView();

    virtual void append(GrView*);
    virtual void prepend(GrView*);
    virtual void insert(ViewIndex, GrView*);
    virtual void remove(ViewIndex);
    virtual void remove(GrView*);

    virtual ViewIndex count() const;
    virtual GrView* child(ViewIndex) const;

protected:
    GrViewList* _body;
};

class GrTarget {
public:
    GrTarget(ViewIndex = 0, Graphic* = nil);
public:
    ViewIndex _index;
    Graphic* _target;
};

declareList(TargetList,GrTarget);

class RootView : public PolyView {
public:
    RootView(
        RootGraphic* = nil, Graphic* = nil, 
        const Color* bg = nil, GlyphViewer* = nil
    );
    virtual ~RootView();

    virtual void setgraphic(Graphic*);

    RootGraphic* getrootgr();
    void background(const Color*);
    const Color* background();
    void viewer(GlyphViewer*);
    GlyphViewer* viewer();
    const TargetList* cur_targets() const;
    
    virtual boolean grasp(const Event&, Tool&, ViewAction*&);
    virtual boolean manipulating(const Event&, Tool&, ViewAction*&);
    virtual void effect(const Event&, Tool&, ViewAction*&);
    
protected:
    RootGraphic* _rootgr;
    TargetList* _targets;
    const Color* _bg;
    GlyphViewer* _gviewer;
    Coord _lx, _ly;
};
inline const Color* RootView::background () { return _bg; }
inline GlyphViewer* RootView::viewer () { return _gviewer; }
inline void RootView::viewer (GlyphViewer* v) { _gviewer = v; }
inline const TargetList* RootView::cur_targets () const{ return _targets; }
inline RootGraphic* RootView::getrootgr () { return _rootgr; }

#endif
