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
 * The metaview customizes the squares frame.
 */

#ifndef metaview_h
#define metaview_h

#include <InterViews/scene.h>

class Button;
class ButtonState;
class Event;
class Interactor;
class Painter;
class Deck;

/*
 * Squares customization information.
 */

enum AdjusterType { AdjustByScrollers, AdjustByPanner };
enum AdjusterSize { Small, Medium, Large };

class SquaresMetaView : public MonoScene {
public:
    AdjusterType type;
    AdjusterSize size;
    Alignment align;
    boolean right;
    boolean below;
    boolean hscroll;
    boolean vscroll;

    SquaresMetaView();
    SquaresMetaView(SquaresMetaView*);
    ~SquaresMetaView();

    boolean Popup(Event&);
private:
    friend class Builder;

    Deck* deck;
    ButtonState* typeButton;
    ButtonState* sizeButton;
    ButtonState* rightButton;
    ButtonState* belowButton;
    ButtonState* alignButton;
    ButtonState* hscrollButton;
    ButtonState* vscrollButton;
    ButtonState* accept;

    void Init();
    void Make();
};

#endif
