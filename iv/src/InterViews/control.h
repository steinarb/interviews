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
 * Controls provide an interface to selecting and executing some action.
 */

#ifndef control_h
#define control_h

#include <InterViews/scene.h>
#include <InterViews/subject.h>

class Control;
class ControlState;

class Control : public MonoScene {
public:
    Control(Interactor*);
    Control(const char* name, Interactor*);
    ~Control();

    ControlState* State() { return state; }
    void SetState(ControlState*);

    virtual void Handle(Event&);

    virtual void Select();	/* highlight, open, and grab */
    virtual void Unselect();	/* close, unhighlight */

    virtual void Do();		/* action for selection */
protected:
    virtual void Down();	/* activate control */
    virtual void Enter();	/* select a control if active */
    virtual void Open();	/* open subviews, if any */
    virtual void Grab();	/* read input events */
    virtual void Skip();	/* ignore input until enters active control */
    virtual void Leave();	/* unselect if active */
    virtual void Close();	/* close subviews, if any */
    virtual void Up();		/* deactivate control, exec selection */

    virtual boolean IsGrabbing(Interactor*);
private:
    ControlState* state;
private:
    void Init(const char*, Interactor*);
};

/*
 * ControlState is a subject that several controls share to exchange
 * common information.
 */

enum ControlStatus { ControlActive = 0x1 };

class ControlState : public Subject {
public:
    ControlState(unsigned status = 0);
    ~ControlState();

    boolean Active() { return Set(ControlActive); }
    void Activate() { Set(ControlActive, true); }
    virtual void Deactivate();

    Control* Selection() { return selection; }
    void Selection(Control* c) { selection = c; }
    virtual void NotifySelection(Control*);

    Control* Action() { return action; }
    void Action(Control* c) { action = c; }

    void Push(ControlState*);
    void Pop();
    ControlState* Next() { return next; }
    ControlState* Prev() { return prev; }
protected:
    unsigned status;
    Control* selection;
    Control* action;
    ControlState* next;
    ControlState* prev;

    boolean Set(ControlStatus s) { return (status & s) != 0; }
    void Set(ControlStatus s, boolean b) {
	status = b ? (status | s) : (status & ~s);
    }
};

#endif
