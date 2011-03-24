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
 * Kit -- object for creating common UI components
 */

#ifndef ivlook_kit_h
#define ivlook_kit_h

#include <InterViews/enter-scope.h>

class Action;
class Adjustable;
class Button;
class Glyph;
class Menu;
class Session;
class String;
class Style;
class Telltale;
class Window;

class Kit {
protected:
    Kit();
public:
    static Kit* instance();
    virtual ~Kit();

    /* beveling for shading */
    virtual Glyph* inset_frame(Glyph*, Style*) const = 0;
    virtual Glyph* outset_frame(Glyph*, Style*) const = 0;
    virtual Glyph* flat_frame(Glyph*, Style*) const = 0;

    /* styled labels */
    virtual Glyph* label(const char*, Style*) const = 0;
    virtual Glyph* label(const String&, Style*) const = 0;

    /* menus */
    virtual Menu* menubar(Style*) const = 0;
    virtual Menu* pulldown(Style*) const = 0;
    virtual Menu* pullright(Style*) const = 0;
    Telltale* simple_menubar_item(const char*, Style*) const;
    Telltale* simple_menubar_item(const String&, Style*) const;
    virtual Telltale* menubar_item(Glyph*, Style*) const = 0;
    Telltale* simple_menu_item(const char*, Style*) const;
    Telltale* simple_menu_item(const String&, Style*) const;
    virtual Telltale* menu_item(Glyph*, Style*) const = 0;
    virtual Telltale* menu_item_separator(Style*) const = 0;
    virtual void popup(Menu*) const = 0;

    /* buttons */
    Button* simple_push_button(const char*, Style*, Action*) const;
    Button* simple_push_button(const String&, Style*, Action*) const;
    virtual Button* push_button(Glyph*, Style*, Action*) const = 0;

    Button* simple_toggle_button(const char*, Style*, Action*) const;
    Button* simple_toggle_button(const String&, Style*, Action*) const;
    virtual Button* toggle_button(Glyph*, Style*, Action*) const = 0;

    Button* simple_radio_button(const char*, Style*, Action*) const;
    Button* simple_radio_button(const String&, Style*, Action*) const;
    virtual Button* radio_button(Glyph*, Style*, Action*) const = 0;

    virtual Action* quit() const = 0;

    /* adjusters */
    virtual Glyph* hscroll_bar(Adjustable*, Style*) const = 0;
    virtual Glyph* vscroll_bar(Adjustable*, Style*) const = 0;
    virtual Glyph* panner(Adjustable*, Adjustable*, Style*) const = 0;

    virtual Button* enlarger(Adjustable*, Style*) const = 0;
    virtual Button* reducer(Adjustable*, Style*) const = 0;
    virtual Button* up_mover(Adjustable*, Style*) const = 0;
    virtual Button* down_mover(Adjustable*, Style*) const = 0;
    virtual Button* left_mover(Adjustable*, Style*) const = 0;
    virtual Button* right_mover(Adjustable*, Style*) const = 0;

    /* Still to come: dialogs, browsers, choosers, editors */
};

inline Telltale* Kit::simple_menubar_item(const char* str, Style* s) const {
    return menubar_item(label(str, s), s);
}

inline Telltale* Kit::simple_menubar_item(const String& str, Style* s) const {
    return menubar_item(label(str, s), s);
}

inline Telltale* Kit::simple_menu_item(const char* str, Style* s) const {
    return menu_item(label(str, s), s);
}

inline Telltale* Kit::simple_menu_item(const String& str, Style* s) const {
    return menu_item(label(str, s), s);
}

inline Button* Kit::simple_push_button(
    const char* str, Style* s, Action* a
) const { return push_button(label(str, s), s, a); }

inline Button* Kit::simple_push_button(
    const String& str, Style* s, Action* a
) const { return push_button(label(str, s), s, a); }

inline Button* Kit::simple_toggle_button(
    const char* str, Style* s, Action* a
) const { return toggle_button(label(str, s), s, a); }

inline Button* Kit::simple_toggle_button(
    const String& str, Style* s, Action* a
) const { return toggle_button(label(str, s), s, a); }

#endif
