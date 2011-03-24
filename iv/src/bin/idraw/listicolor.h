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

// $Header: listicolor.h,v 1.9 89/10/09 14:48:48 linton Exp $
// declares classes IColorNode and IColorList.

#ifndef listicolor_h
#define listicolor_h

#include "list.h"

// Declare imported types.

class IColor;

// An IColorNode contains an IColor pointer.

class IColorNode : public BaseNode {
public:

    IColorNode (IColor* c) { color = c; }
    boolean SameValueAs (void* p) { return color == p; }
    IColor* GetColor () { return color; }

protected:

    IColor* color;		// points to an IColor

};

// An IColorList manages a list of IColorNodes.

class IColorList : public BaseList {
public:

    IColorNode* First();
    IColorNode* Last();
    IColorNode* Prev();
    IColorNode* Next();
    IColorNode* GetCur();
    IColorNode* Index(int);

};

// Cast these functions to return IColorNodes instead of BaseNodes.

inline IColorNode* IColorList::First () {
    return (IColorNode*) BaseList::First();
}

inline IColorNode* IColorList::Last () {
    return (IColorNode*) BaseList::Last();
}

inline IColorNode* IColorList::Prev () {
    return (IColorNode*) BaseList::Prev();
}

inline IColorNode* IColorList::Next () {
    return (IColorNode*) BaseList::Next();
}

inline IColorNode* IColorList::GetCur () {
    return (IColorNode*) BaseList::GetCur();
}

inline IColorNode* IColorList::Index (int index) {
    return (IColorNode*) BaseList::Index(index);
}

#endif
