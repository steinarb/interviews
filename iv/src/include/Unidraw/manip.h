/*
 * Copyright (c) 1990, 1991 Stanford University
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
 * Manipulator - base class for defining direct manipulation semantcs.
 */

#ifndef unidraw_manip_h
#define unidraw_manip_h

#include <InterViews/enter-scope.h>
#include <Unidraw/enter-scope.h>

#include <InterViews/_enter.h>

class Event;
class Tool;
class Viewer;

//: base class for defining direct-manipulation semantics.
// <a href=../man3.1/Manipulator.3.1>man page</a>
class Manipulator {
public:
    virtual ~Manipulator();
    
    virtual void Grasp(Event&);
    virtual boolean Manipulating(Event&);
    virtual void Effect(Event&);

    virtual void SetViewer(Viewer*);
    virtual void SetTool(Tool*);

    virtual Viewer* GetViewer();
    virtual Tool* GetTool();
protected:
    Manipulator();
};

#include <InterViews/_leave.h>

#endif
