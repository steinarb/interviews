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

// $Header: listibrush.h,v 1.9 89/10/09 14:48:46 linton Exp $
// declares classes IBrushNode and IBrushList.

#ifndef listibrush_h
#define listibrush_h

#include "list.h"

// Declare imported types.

class IBrush;

// An IBrushNode contains an IBrush pointer.

class IBrushNode : public BaseNode {
public:

    IBrushNode (IBrush* b) { brush = b; }
    boolean SameValueAs (void* p) { return brush == p; }
    IBrush* GetBrush () { return brush; }

protected:

    IBrush* brush;		// points to an IBrush

};

// An IBrushList manages a list of IBrushNodes.

class IBrushList : public BaseList {
public:

    IBrushNode* First();
    IBrushNode* Last();
    IBrushNode* Prev();
    IBrushNode* Next();
    IBrushNode* GetCur();
    IBrushNode* Index(int);

};

// Cast these functions to return IBrushNodes instead of BaseNodes.

inline IBrushNode* IBrushList::First () {
    return (IBrushNode*) BaseList::First();
}

inline IBrushNode* IBrushList::Last () {
    return (IBrushNode*) BaseList::Last();
}

inline IBrushNode* IBrushList::Prev () {
    return (IBrushNode*) BaseList::Prev();
}

inline IBrushNode* IBrushList::Next () {
    return (IBrushNode*) BaseList::Next();
}

inline IBrushNode* IBrushList::GetCur () {
    return (IBrushNode*) BaseList::GetCur();
}

inline IBrushNode* IBrushList::Index (int index) {
    return (IBrushNode*) BaseList::Index(index);
}

#endif
