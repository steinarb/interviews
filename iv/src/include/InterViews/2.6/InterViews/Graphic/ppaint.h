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
 * Persistent graphics attributes.
 */

#ifndef ppaint_h
#define ppaint_h

#include <InterViews/defs.h>
#include <InterViews/brush.h>
#include <InterViews/color.h>
#include <InterViews/font.h>
#include <InterViews/pattern.h>
#include <InterViews/Graphic/objman.h>
#include <InterViews/Graphic/pfile.h>
#include <InterViews/Graphic/persistent.h>
#include <InterViews/Graphic/ref.h>
#include <InterViews/Graphic/reflist.h>

#ifdef ColorIntensity
#undef ColorIntensity
#endif

typedef int ColorIntensity;

class PColor : public Persistent {
public:
    PColor();
    PColor(ColorIntensity r, ColorIntensity g, ColorIntensity b);
    PColor(const char*);
    ~PColor();

    int PixelValue ();
    void Intensities(ColorIntensity& r, ColorIntensity& g, ColorIntensity& b);
    boolean Valid ();
    operator const Color*();

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
protected:
    virtual boolean read(PFile*);
    virtual boolean write(PFile*);
protected:
    const Color* value;
};

static const int patternWidth = 16;
static const int patternHeight = 16;

class PPattern : public Persistent {
public:
    PPattern();
    PPattern(int p[patternHeight]);
    PPattern(int dither);
    ~PPattern();

    operator Pattern*();

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
protected:
    virtual boolean read(PFile*);
    virtual boolean write(PFile*);
protected:
    int data [patternHeight];
    Pattern* value;
};

static const int NO_WIDTH = -1;

class PBrush : public Persistent {
public:
    PBrush();
    PBrush(int p, int w = 1);
    ~PBrush();

    int Width();
    operator Brush*();

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
protected:
    virtual boolean read(PFile*);
    virtual boolean write(PFile*);
protected:
    int p;
    Brush* value;
};

class PFont : public Persistent {
public:
    PFont();
    PFont(const char*);
    PFont(const char*, int);
    ~PFont();

    int Baseline();
    int Height();
    int Width(const char*);
    int Width(const char*, int);
    int Index(const char*, int offset, boolean between);
    int Index(const char*, int, int offset, boolean between);

    boolean Valid();
    boolean FixedWidth();
    operator const Font*();

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
protected:
    virtual boolean read(PFile*);
    virtual boolean write(PFile*);
protected:
    char* name;
    int count;
    const Font* value;
};

/*
 * Standard attributes.  These must be initialized by calling
 * InitPPaint.  If the persistence features of paints are not required,
 * then InitPPaint must be called explicitly before any of the standard
 * attributes are used.  Otherwise the call to InitPPaint should be
 * made in the persistent object initialization routine passed to the
 * object manager constructor.  See Persistent(3I) for more information.
 */
extern PColor* pblack;
extern PColor* pwhite;
extern PPattern* psolid;
extern PPattern* pclear;
extern PBrush* psingle;
extern PFont* pstdfont;
extern void InitPPaint();

/*
 * inlines
 */

inline int PColor::PixelValue () { return value->PixelValue(); }
inline boolean PColor::Valid () { return value != nil; }
inline PColor::operator const Color* () { return value; }

inline void PColor::Intensities (
    ColorIntensity& r, ColorIntensity& g, ColorIntensity& b
) {
    value->Intensities(r, g, b);
}

inline PPattern::operator Pattern* () { return value; }

inline int PBrush::Width () { 
    return (value == nil) ? NO_WIDTH : value->Width();
}

inline PBrush::operator Brush* () { return value; }

inline int PFont::Baseline () { return value->Baseline(); }
inline int PFont::Height () { return value->Height(); }
inline int PFont::Width (const char* s) { return value->Width(s); }
inline int PFont::Width (const char* s, int len) {return value->Width(s, len);}
inline boolean PFont::Valid () { return value != nil; }
inline boolean PFont::FixedWidth () { return value->FixedWidth(); }
inline PFont::operator const Font* () { return value; }

#endif
