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
 * Menu - a visible list of Actions
 */

#ifndef ivlook_menu_h
#define ivlook_menu_h

#include <InterViews/handler.h>
#include <InterViews/monoglyph.h>

class Action;
class Listener;
class Menu;
class MenuItemList;
class Patch;
class Telltale;
class Window;

class Menu : public MonoGlyph, public Handler {
public:
    Menu(Glyph*, float x1, float y1, float x2, float y2);
    virtual ~Menu();

    virtual void body(Glyph*);
    virtual Glyph* body() const;

    virtual void item(GlyphIndex, Telltale*, Action*, Menu*, Window* = nil);
    virtual void add_item(Telltale*, Action*, Menu*, Window* = nil);
    virtual void add_item(Telltale*, Action*);
    virtual void add_item(Telltale*, Menu*, Window* = nil);
    virtual void add_item(Telltale*);

    virtual Telltale* telltale(GlyphIndex) const;
    virtual Action* action(GlyphIndex) const;
    virtual Menu* menu(GlyphIndex) const;
    virtual Window* window(GlyphIndex) const;

    void select(GlyphIndex);

    virtual void event(Event&);
private:
    MenuItemList* itemlist_;
    GlyphIndex item_;
    float x1_, y1_, x2_, y2_;

    Glyph* contents_;
    Listener* listener_;
    Patch* patch_;
};

#endif
