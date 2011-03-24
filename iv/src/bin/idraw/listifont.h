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

// $Header: listifont.h,v 1.9 89/10/09 14:48:50 linton Exp $
// declares classes IFontNode and IFontList.

#ifndef listifont_h
#define listifont_h

#include "list.h"

// Declare imported types.

class IFont;

// An IFontNode contains an IFont pointer.

class IFontNode : public BaseNode {
public:

    IFontNode (IFont* f) { font = f; }
    boolean SameValueAs (void* p) { return font == p; }
    IFont* GetFont () { return font; }

protected:

    IFont* font;		// points to an IFont

};

// An IFontList manages a list of IFontNodes.

class IFontList : public BaseList {
public:

    IFontNode* First();
    IFontNode* Last();
    IFontNode* Prev();
    IFontNode* Next();
    IFontNode* GetCur();
    IFontNode* Index(int);

};

// Cast these functions to return IFontNodes instead of BaseNodes.

inline IFontNode* IFontList::First () {
    return (IFontNode*) BaseList::First();
}

inline IFontNode* IFontList::Last () {
    return (IFontNode*) BaseList::Last();
}

inline IFontNode* IFontList::Prev () {
    return (IFontNode*) BaseList::Prev();
}

inline IFontNode* IFontList::Next () {
    return (IFontNode*) BaseList::Next();
}

inline IFontNode* IFontList::GetCur () {
    return (IFontNode*) BaseList::GetCur();
}

inline IFontNode* IFontList::Index (int index) {
    return (IFontNode*) BaseList::Index(index);
}

#endif
