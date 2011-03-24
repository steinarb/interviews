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
 * A frame surrounds the squares view with banner and scroll bars.
 */

#ifndef sframe_h
#define sframe_h

#include <InterViews/scene.h>

class Menu;
class SquaresMetaView;
class SquaresView;
class Viewport;

class SquaresFrame : public MonoScene {
public:
    SquaresFrame(SquaresView*);
    ~SquaresFrame();

    virtual void Handle(Event&);

    void AddSquare();
    void NewView();
    void ViewSetup();
    void Close();
    void Quit();
    void ZoomIn();
    void ZoomOut();
    void NormalSize();
    void CenterView();
    void YesQuit();
    void NoQuit();
private:
    static int nviews;
    SquaresView* view;
    Viewport* viewport;
    Menu* menu;
    Menu* adjust;
    Menu* quit;
    SquaresMetaView* style;
private:
    SquaresFrame(SquaresFrame*);
    void Init();
    Menu* MakeMenu(Menu*, struct MenuInfo*);
    void MakeFrame();
    Interactor* ScrollerFrameInterior();
    Interactor* PannerFrameInterior();
};

#endif
