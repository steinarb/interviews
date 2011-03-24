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

// $Header: ipaint.h,v 1.10 89/10/09 14:48:20 linton Exp $
// declares classes IBrush, IFont, and IPattern.

#ifndef ipaint_h
#define ipaint_h

#include <InterViews/Graphic/ppaint.h>

// An IBrush knows how to test for its noneness and get its line
// pattern, width, arrows, and dash pattern.

class IBrush : public PBrush {
public:

    IBrush();
    IBrush(int, int, boolean, boolean);

    boolean None();
    int GetLinePattern();
    int Width();
    boolean LeftArrow();
    boolean RightArrow();
    const int* GetDashPattern();
    int GetDashPatternSize();
    int GetDashOffset();
    operator Brush*();

protected:

    void CalcDashPat(int);

    boolean leftarrow;		// stores true if line starts from an arrowhead
    boolean rightarrow;		// stores true if line ends in an arrowhead
    int dashpat[patternWidth];	// stores dash pattern
    int dashpatsize;		// stores number of defined elements in dashpat
    int dashoffset;		// stores dash pattern's offset

};

// Define inline access functions to get members' values.

inline boolean IBrush::None () {
    return (value == nil);
}

inline boolean IBrush::LeftArrow () {
    return leftarrow;
}

inline boolean IBrush::RightArrow () {
    return rightarrow;
}

inline int IBrush::GetLinePattern () {
    return p;
}

inline const int* IBrush::GetDashPattern () {
    return dashpat;
}

inline int IBrush::GetDashPatternSize () {
    return dashpatsize;
}

inline int IBrush::GetDashOffset () {
    return dashoffset;
}

inline IBrush::operator Brush* () {
    return value;
}

// An IColor knows how to get its name.

class IColor : public PColor {
public:

    IColor(const char*);
    IColor(int, int, int, const char*);
    IColor(Color*, const char*);
    ~IColor();

    const char* GetName();
    operator Color*();

protected:

    char* name;			// stores name passed into constructor

};

// Define inline access functions to get members' values.

inline const char* IColor::GetName () {
    return name;
}

inline IColor::operator Color* () {
    return value;
}

// An IFont knows how to get its name, print font, and print size.

class IFont : public PFont {
public:

    IFont(const char*, const char*, const char*);
    ~IFont();

    const char* GetName();
    const char* GetPrintFont();
    const char* GetPrintSize();
    const char* GetPrintFontAndSize();
    int GetLineHt();
    operator Font*();

protected:

    const char* FilterName(const char*);

    char* printfont;		// stores name of font used by printer
    char* printsize;		// stores scale of font used by printer
    char* printfontandsize;	// stores name and size separated by a blank
    int lineHt;			// stores printsize converted to int

};

// Define inline access functions to get members' values.

inline const char* IFont::GetName () {
    return name ? name : "stdfont";
}

inline const char* IFont::GetPrintFont () {
    return printfont;
}

inline const char* IFont::GetPrintSize () {
    return printsize;
}

inline const char* IFont::GetPrintFontAndSize () {
    return printfontandsize;
}

inline int IFont::GetLineHt () {
    return lineHt;
}

inline IFont::operator Font* () {
    return value;
}

// An IPattern knows how to test for its noneness or fullness and get
// its dither, data, and gray level.

class IPattern : public PPattern {
public:

    IPattern();
    IPattern(int, float);
    IPattern(int pattern[patternHeight], int);

    boolean None();
    float GetGrayLevel();
    const int* GetData();
    int GetSize();
    operator Pattern*();

protected:

    float graylevel;		// stores gray level for grayscale patterns
    int size;			// stores pat's orig size (4x4, 8x8, or 16x16)

};

// Define inline access functions to get members' values.

inline boolean IPattern::None () {
    return (value == nil);
}

inline float IPattern::GetGrayLevel () {
    return graylevel;
}

inline const int* IPattern::GetData () {
    return data;
}

inline int IPattern::GetSize () {
    return size;
}

inline IPattern::operator Pattern* () {
    return value;
}

#endif
