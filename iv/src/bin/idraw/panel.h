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

// $Header: panel.h,v 1.9 89/10/09 14:49:13 linton Exp $
// declares classes Panel and PanelItem.

#ifndef panel_h
#define panel_h

#include "highlighter.h"

// Declare imported and used-before-defined types.

class PanelItem;

// A Panel displays several items but highlights only one item.

class Panel : public HighlighterParent {
public:

    Panel();

    void Enter(PanelItem*, char);
    PanelItem* LookUp(char);

    void SetCur(PanelItem*);
    PanelItem* GetCur();

    void PerformCurrentFunction(Event&);
    void PerformTemporaryFunction(Event&, char);

protected:

    PanelItem* cur;		// stores currently selected item
    PanelItem* items[128];	// stores items by their associated character

};

// A PanelItem displays two text labels and performs a function.

class PanelItem : public Highlighter {
public:

    PanelItem(Panel*, const char*, const char*, char);
    ~PanelItem();

    void Handle(Event&);

    virtual void Perform(Event&);

protected:

    void Reconfig();
    void Redraw(Coord, Coord, Coord, Coord);
    void Resize();

    Panel* panel;		// stores panel which this item belongs to
    char* name;			// stores item's text label
    char* key;			// stores label of key which selects this item

    Coord name_x, name_y;	// stores position at which to display name
    Coord key_x, key_y;		// stores position at which to display key
    Coord side;			// size of largest square fitting in canvas
    Coord offx;			// horizontal offset needed to center square
    Coord offy;			// vertical offset needed to center square

};

#endif
