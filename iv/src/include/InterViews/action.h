/*
 * Copyright (c) 1987, 1988, 1989, 1990, 1991 Stanford University
 * Copyright (c) 1991 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and 
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Stanford and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Stanford and Silicon Graphics.
 * 
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 *
 * IN NO EVENT SHALL STANFORD OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
 * OF THIS SOFTWARE.
 */

/*
 * Action - something to do
 */

#ifndef iv_action_h
#define iv_action_h

#include <InterViews/resource.h>

#include <InterViews/_enter.h>

class Action : virtual public Resource {
protected:
    Action();
    virtual ~Action();
public:
    virtual void execute() = 0;
};

/*
 * Macro - list of actions
 */

class Macro : public Action {
public:
    Macro(Action* = nil, Action* = nil, Action* = nil, Action* = nil);
    virtual ~Macro();

    void append(Action*);

    virtual void execute();
protected:
    Action** actions_;
    int count_;
};

/*
 * Action denoted by an object and member function to call on the object.
 */

#include <generic.h>

#define ActionCallback(T) name2(T,_ActionCallback)
#define ActionMemberFunction(T) name2(T,_ActionMemberFunction)

#define ActionCallbackdeclare(T) \
typedef void T::ActionMemberFunction(T)(); \
class ActionCallback(T) : public Action { \
public: \
    ActionCallback(T)(T*, ActionMemberFunction(T)*); \
\
    virtual void execute(); \
private: \
    T* obj_; \
    ActionMemberFunction(T)* func_; \
};

#define ActionCallbackimplement(T) \
ActionCallback(T)::ActionCallback(T)(T* obj, ActionMemberFunction(T)* func) { \
    obj_ = obj; \
    func_ = func; \
} \
\
void ActionCallback(T)::execute() { (obj_->*func_)(); }

#include <InterViews/_leave.h>

#endif
