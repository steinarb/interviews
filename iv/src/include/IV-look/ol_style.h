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
 * OpenLook style information
 */

#ifndef ivlook_ol_style_h
#define ivlook_ol_style_h

#include <InterViews/boolean.h>
#include <InterViews/resource.h>

class Color;
class Font;
class OL_StyleImpl;
class Style;

class OL_Style : virtual public Resource {
public:
    OL_Style(Style*);
    ~OL_Style();

    const Color* foreground() const;
    const Color* background() const;
    const Color* shadow() const;
    const Color* bright() const;
    const Color* dull() const;
    const Color* fill_out() const;
    const Color* fill_in() const;
    const Color* gray_out() const;
    const Color* gray_in() const;
    const Font* glyph_font() const;
private:
    OL_StyleImpl* impl_;
};

#endif
