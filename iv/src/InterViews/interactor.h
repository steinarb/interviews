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
 * Base class for interactive objects.
 */

#ifndef interactor_h
#define interactor_h

#include <InterViews/defs.h>

/* these are obsolete and should not be used */
extern class Sensor* stdsensor;
extern class Painter* stdpaint;

enum CanvasType {
    CanvasShapeOnly, CanvasInputOnly, CanvasInputOutput,
    CanvasSaveUnder, CanvasSaveContents, CanvasSaveBoth
};

enum InteractorType {
    InteriorInteractor, PopupInteractor, TransientInteractor,
    ToplevelInteractor, ApplicationInteractor, IconInteractor
};

class Bitmap;
class Canvas;
class Cursor;
class Event;
class Perspective;
class Scene;
class Shape;
class StringId;
class TopLevel;
class World;

class Interactor {
public:
    Interactor();
    Interactor(const char*);
    virtual ~Interactor();

    /* configuration */
    void Align(Alignment, int w, int h, Coord& l, Coord& b);
    void Config(Scene* = nil);
    virtual void Reconfig();
    const char* GetAttribute(const char*);
    virtual void Reshape(Shape&);
    Shape* GetShape();
    void SetCursor(Cursor*);
    Cursor* GetCursor();
    const char* GetClassName();
    const char* GetInstance();

    /* traversing hierarchy */
    virtual void GetComponents(Interactor**, int, Interactor**&, int&);
    void GetRelative(Coord& x, Coord &y, Interactor* = nil);
    Scene* Parent();
    Scene* Root();
    World* GetWorld();

    /* output */
    Canvas* GetCanvas();
    virtual void Draw();
    virtual void Highlight(boolean on);

    /* input events */
    boolean Check();
    int CheckQueue();
    void Flush();
    virtual void Handle(Event&);
    void Listen(Sensor*);
    void Poll(Event&);
    void Read(Event&);
    void Run();
    void QuitRunning(Event&);
    void Sync();
    void UnRead(Event&);

    /* subject-view communication */
    virtual void Adjust(Perspective&);
    Perspective* GetPerspective();
    virtual void Update();

    /* top-level interactors */
    void SetName(const char*);
    const char* GetName();
    void SetGeometry(const char*);
    const char* GetGeometry();
    void SetCanvasType(CanvasType);
    CanvasType GetCanvasType();
    void SetInteractorType(InteractorType);
    InteractorType GetInteractorType();
    void SetGroupLeader(Interactor*);
    Interactor* GetGroupLeader();
    void SetTransientFor(Interactor*);
    Interactor* GetTransientFor();

    /* icons */
    void SetIconName(const char*);
    const char* GetIconName();

    void SetIconInteractor(Interactor*);
    Interactor* GetIconInteractor();

    void SetIconGeometry(const char*);
    const char* GetIconGeometry();

    void SetIconBitmap(Bitmap*);
    Bitmap* GetIconBitmap();

    void SetIconMask(Bitmap*);
    Bitmap* GetIconMask();

    void SetStartIconic(boolean);
    boolean GetStartIconic();

    void PlaceIcon(Interactor*, Canvas*&);

    void Iconify();
    void DeIconify();

    /* obsolete -- use Set/GetIconInteractor */
    void SetIcon(Interactor*);
    Interactor* GetIcon();
protected:
    Shape* shape;			/* desired shape characteristics */
    Canvas* canvas;			/* actual display area */
    Perspective* perspective;		/* portion displayed */
    Coord xmax;				/* canvas->Width() - 1 */
    Coord ymax;				/* canvas->Height() - 1 */
    Sensor* input;			/* normal input event interest */
    Painter* output;			/* normal output parameters */

    Interactor(Sensor*, Painter*);	/* old-fashioned constructor */

    virtual void Redraw(Coord left, Coord bottom, Coord right, Coord top);
    virtual void RedrawList(int n, Coord[], Coord[], Coord[], Coord[]);
    virtual void Resize();

    virtual void Activate();
    virtual void Deactivate();

    void SetClassName(const char*);
    void SetInstance(const char*);
    void RootConfig();
    boolean IsMapped();
private:
    friend class Scene;
    friend class Canvas;
    friend class World;

    Scene* parent;			/* where inserted */
    StringId* classname;		/* class for attributes */
    StringId* instance;			/* instance for attributes */
    TopLevel* toplevel;			/* top-level interactor parameters */
    Coord left;				/* relative to parent */
    Coord bottom;			/* relative to parent */
    Sensor* cursensor;			/* current input interest */

    void Init();
    void DefaultConfig(boolean&);
    void DoConfig(boolean);

    TopLevel* GetTopLevel();
    void DoSetCursor(Cursor*);
    void DoSetName(const char*);
    void DoSetGroupLeader(Interactor*);
    void DoSetTransientFor(Interactor*);
    void DoSetIconName(const char*);
    void DoSetIconInteractor(Interactor*);
    void DoSetIconGeometry(const char*);
    void DoSetIconBitmap(Bitmap*);
    void DoSetIconMask(Bitmap*);

    int Fileno();

    void DiscardUnreadEvents();
    boolean Select(Event&);
    virtual boolean GetEvent(Event&, boolean);
    void SendRedraw(Coord x, Coord y, int width, int height, int count);
    void SendResize(Coord x, Coord y, int width, int height);
    void SendActivate();
    void SendDeactivate();
};

inline Canvas* Interactor::GetCanvas() { return canvas; }
inline Scene* Interactor::Parent () { return parent; }
inline Perspective* Interactor::GetPerspective () { return perspective; }
inline Shape* Interactor::GetShape () { return shape; }

inline void Interactor::SetIcon (Interactor* i) { SetIconInteractor(i); }
inline Interactor* Interactor::GetIcon () { return GetIconInteractor(); }

#endif
