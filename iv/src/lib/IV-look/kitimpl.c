/*
 * Copyright (c) 1991 Stanford University
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
 * KitImpl -- common kit implementation support
 */

#include <IV-look/adjustable.h>
#include <IV-look/kitimpl.h>
#include <InterViews/session.h>

implement(ActionCallback,Session);

MoverAction::MoverAction(Adjustable* a, DimensionName d) {
    adjustable_ = a;
    dimension_ = d;
}

MoverAction::~MoverAction() { }

void MoverAction::execute() { move(adjustable_, dimension_); }

MoverActionForward::MoverActionForward(
    Adjustable* a, DimensionName d
) : MoverAction(a, d) { }

MoverActionForward::~MoverActionForward() { }

void MoverActionForward::move(Adjustable* a, DimensionName d) const {
    a->scroll_forward(d);
}

MoverActionBackward::MoverActionBackward(
    Adjustable* a, DimensionName d
) : MoverAction(a, d) { }

MoverActionBackward::~MoverActionBackward() { }

void MoverActionBackward::move(Adjustable* a, DimensionName d) const {
    a->scroll_backward(d);
}
