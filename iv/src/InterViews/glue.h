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
 * Glue is useful for variable spacing between interactors.
 */

#ifndef glue_h
#define glue_h

#include <InterViews/interactor.h>
#include <InterViews/shape.h>

class Glue : public Interactor {
protected:
    Glue();
    Glue(const char*);
    Glue(Painter* bg);

    void Redraw(Coord, Coord, Coord, Coord);
private:
    void Init();
};

class HGlue : public Glue {
public:
    HGlue(int natural = 0, int stretch = hfil);
    HGlue(const char*, int natural = 0, int stretch = hfil);
    HGlue(int natural, int shrink, int stretch);
    HGlue(const char*, int natural, int shrink, int stretch);
    HGlue(Painter* bg, int natural = 0, int stretch = hfil);
    HGlue(Painter* bg, int natural, int shrink, int stretch);
private:
    void Init(int nat, int shrink, int stretch);
};

class VGlue : public Glue {
public:
    VGlue(int natural = 0, int stretch = vfil);
    VGlue(const char*, int natural = 0, int stretch = vfil);
    VGlue(int natural, int shrink, int stretch);
    VGlue(const char*, int natural, int shrink, int stretch);
    VGlue(Painter* bg, int natural = 0, int stretch = vfil);
    VGlue(Painter* bg, int natural, int shrink, int stretch);
private:
    void Init(int nat, int shrink, int stretch);
};

#endif
