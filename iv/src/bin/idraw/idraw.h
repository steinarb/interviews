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

// $Header: idraw.h,v 1.8 89/10/09 14:48:10 linton Exp $
// declares class Idraw.

#ifndef idraw_h
#define idraw_h

#include <InterViews/scene.h>

// Declare imported types.

class Drawing;
class DrawingView;
class Editor;
class ErrHandler;
class MapKey;
class State;

// An Idraw displays a drawing editor.

class Idraw : public MonoScene {
public:

    Idraw(int, char**);
    ~Idraw();

    void Run();

    void Handle(Event&);
    void Update();

protected:

    void ParseArgs(int, char**);
    void Init();

    const char* initialfile;	// stores name of initial file to open if any

    Drawing* drawing;		// performs operations on drawing
    DrawingView* drawingview;	// displays drawing
    Editor* editor;		// handles drawing and editing operations
    ErrHandler* errhandler;	// handles an X request error
    MapKey* mapkey;		// maps characters to Interactors
    State* state;		// stores current state info about drawing
    Interactor* tools;		// displays drawing tools

};

#endif
