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
 * X11-dependent Display representation.
 */

#ifndef iv_xdisplay_h
#define iv_xdisplay_h

#include <InterViews/display.h>
#include <OS/enter-scope.h>
#include <IV-X11/Xlib.h>
#include <IV-X11/Xutil.h>

#include <InterViews/_enter.h>

class ColorTable;
class DamageList;
class GrabList;
class RGBTable;
class String;
class Transformer;
class WindowTable;

class DisplayRep {
public:
    XDisplay* display_;
    unsigned int screen_;
    unsigned int depth_;
    XWindow root_;
    Visual* visual_;
    Coord width_;
    Coord height_;
    unsigned int pwidth_;
    unsigned int pheight_;
    Style* style_;

    XColormap cmap_;
    ColorTable* ctable_;
    RGBTable* rgbtable_;
    XColor* localmap_;
    unsigned int localmapsize_;
    unsigned long red_;
    unsigned long red_shift_;
    unsigned long green_;
    unsigned long green_shift_;
    unsigned long blue_;
    unsigned long blue_shift_;
    unsigned long white_;

    unsigned long xor_;
    GrabList* grabbers_;
    DamageList* damage_list_;
    WindowTable* wtable_;

    void init_style();

    void set_visual();
    void set_visual_by_class_name(const String&);
    void set_visual_by_info(XVisualInfo&, long mask);
    void set_color_tables();
    void set_shift(unsigned long mask, unsigned long& v, unsigned long& shift);
    void set_shaped_windows();
    void set_synchronous();
    void set_xor_pixel();
    void set_dpi(Coord&);

    void find_color(unsigned long, XColor&);
    void find_color(
	unsigned short r, unsigned short g, unsigned short b, XColor&
    );
};

#include <InterViews/_leave.h>

#endif
