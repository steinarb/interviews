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

#include <InterViews/action.h>

Action::Action() { }
Action::~Action() { }

Macro::Macro(Action* a0, Action* a1, Action* a2, Action* a3) : Action() {
    actions_ = new Action*[10];
    append(a0);
    append(a1);
    append(a2);
    append(a3);
}

Macro::~Macro() {
    for (int i = 0; i < count_; ++i) {
        if (actions_[i] != nil) {
            actions_[i]->unref();
        }
    }
    delete actions_;
}

void Macro::append(Action* action) {
    if (action != nil) {
        actions_[count_] = action;
        actions_[count_]->ref();
        ++count_;
    }
}

void Macro::execute() {
    for (int i = 0; i < count_; ++i) {
        actions_[i]->execute();
    }
}
