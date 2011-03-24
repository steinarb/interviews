/*
 * Copyright (c) 1991 Stanford University
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
 * PagenumberView
 */

#ifndef PagenumberView_h
#define PagenumberView_h

#include "ItemView.h"

class Patch;
class PagenumberItem;

class PagenumberView : public ItemView {
public:
    PagenumberView (DocumentViewer*, ItemView*, PagenumberItem*);

    virtual void draw (Canvas*, const Allocation&) const;
    virtual void print (Printer*, const Allocation&) const;
protected:
    virtual ~PagenumberView ();

    PagenumberItem* _pagenumber;
    Coord _width;
    char* _label;
    Patch* _patch;
};

#endif
