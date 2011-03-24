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
 * Glue is useful for variable spacing between interactors.
 */

#ifndef iv2_6_glue_h
#define iv2_6_glue_h

#include <IV-2_6/InterViews/interactor.h>
#include <IV-2_6/InterViews/shape.h>

#include <IV-2_6/_enter.h>

class Glue : public Interactor {
protected:
    Glue();
    Glue(const char*);
    virtual ~Glue();

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
    virtual ~HGlue();
private:
    void Init(int nat, int shrink, int stretch);
};

class VGlue : public Glue {
public:
    VGlue(int natural = 0, int stretch = vfil);
    VGlue(const char*, int natural = 0, int stretch = vfil);
    VGlue(int natural, int shrink, int stretch);
    VGlue(const char*, int natural, int shrink, int stretch);
    virtual ~VGlue();
private:
    void Init(int nat, int shrink, int stretch);
};

#include <IV-2_6/_leave.h>

#endif
