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

/*
 * A color is defined by rgb intensities and usually accessed by name.
 */

#ifndef color_h
#define color_h

#include <InterViews/resource.h>

/*
 * Color intensity should be subrange 0..65535, but for compatibility
 * reasons it is an integer.
 */
typedef int ColorIntensity;

class Color : public Resource {
public:
    Color(ColorIntensity r, ColorIntensity g, ColorIntensity b);
    Color(const char*);
    Color(const char*, int length);
    Color(int pixel);
    ~Color();

    void Intensities(ColorIntensity& r, ColorIntensity& g, ColorIntensity& b);
    void DisplayIntensities(
        ColorIntensity& r, ColorIntensity& g, ColorIntensity& b
    );
    int PixelValue();
    boolean Valid();
protected:
    class ColorRep* rep;
    ColorIntensity red, green, blue;
};

class ColorRep {
private:
    friend class Color;

    ColorRep(int pixel, ColorIntensity&, ColorIntensity&, ColorIntensity&);
    ColorRep(const char*, ColorIntensity&, ColorIntensity&, ColorIntensity&);
    ColorRep(ColorIntensity, ColorIntensity, ColorIntensity);
    ~ColorRep();

    int GetPixel();
    void GetIntensities(ColorIntensity&, ColorIntensity&, ColorIntensity&);

    void* info;
};

extern Color* black;
extern Color* white;

inline void Color::Intensities (
    ColorIntensity& r, ColorIntensity& g, ColorIntensity& b
) {
    r = red; g = green; b = blue;
}

inline void Color::DisplayIntensities (
    ColorIntensity& r, ColorIntensity& g, ColorIntensity& b
) {
    rep->GetIntensities(r, g, b);
}

inline int Color::PixelValue () { return rep->GetPixel(); }

#endif
