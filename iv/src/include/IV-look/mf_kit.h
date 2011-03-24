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
 * MFKit -- object for creating common UI Motif-ish components
 */

#ifndef ivlook_mfkit_h
#define ivlook_mfkit_h

#include <IV-look/kit.h>

class MFKitImpl;

class MFKit : public Kit {
public:
    MFKit();
    virtual ~MFKit();

    /* beveling for shading */
    virtual Glyph* inset_frame(Glyph*, Style*) const;
    virtual Glyph* outset_frame(Glyph*, Style*) const;
    virtual Glyph* flat_frame(Glyph*, Style*) const;

    /* styled labels */
    virtual Glyph* label(const char*, Style*) const;
    virtual Glyph* label(const String&, Style*) const;

    /* menus */
    virtual Menu* menubar(Style*) const;
    virtual Menu* pulldown(Style*) const;
    virtual Menu* pullright(Style*) const;
    virtual Telltale* menubar_item(Glyph*, Style*) const;
    virtual Telltale* menu_item(Glyph*, Style*) const;
    virtual Telltale* menu_item_separator(Style*) const;
    virtual void popup(Menu*) const;

    /* buttons */
    virtual Button* push_button(Glyph*, Style*, Action*) const;
    virtual Button* toggle_button(Glyph*, Style*, Action*) const;
    virtual Button* radio_button(Glyph*, Style*, Action*) const;

    virtual Action* quit() const;

    /* adjusters */
    virtual Glyph* hscroll_bar(Adjustable*, Style*) const;
    virtual Glyph* vscroll_bar(Adjustable*, Style*) const;
    virtual Glyph* panner(Adjustable*, Adjustable*, Style*) const;

    virtual Button* enlarger(Adjustable*, Style*) const;
    virtual Button* reducer(Adjustable*, Style*) const;
    virtual Button* up_mover(Adjustable*, Style*) const;
    virtual Button* down_mover(Adjustable*, Style*) const;
    virtual Button* left_mover(Adjustable*, Style*) const;
    virtual Button* right_mover(Adjustable*, Style*) const;
private:
    MFKitImpl* impl_;
};

#endif
