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
 * A font is essentially an array of similar stencils that can be
 * indexed by a code corresponding to a character in an alphabet.
 */

#ifndef iv_font_h
#define iv_font_h

#include <InterViews/boolean.h>
#include <InterViews/coord.h>
#include <InterViews/resource.h>

#include <InterViews/_enter.h>

class Display;
class FontImpl;
class FontRep;
class String;

/*
 * FontFamily - manages related fonts
 */

class FontFamily {
public:
    FontFamily(const char* familyname);
    virtual ~FontFamily();

    virtual boolean font(int size, const char*& name, float& scale) const;
    virtual boolean font(
        int size, const char* style, const char*& name, float& scale
    ) const;

    class FontFamilyRep* rep(Display*) const;
private:
    class FontFamilyImpl* impl_;

    FontFamilyRep* create(Display*) const;
    void destroy(FontFamilyRep*);
};

class Font : virtual public Resource {
public:
    Font(const String&, float scale = 1.0);
    Font(const char*, float scale = 1.0);
    virtual ~Font();

    static const Font* lookup(const String&);
    static const Font* lookup(const char*);

    static boolean exists(Display*, const String&);
    static boolean exists(Display*, const char*);

    virtual const char* name() const;
    virtual const char* encoding() const;
    virtual Coord size() const;

    virtual Coord ascent() const;
    virtual Coord descent() const;

    virtual Coord left_bearing(long) const;
    virtual Coord right_bearing(long) const;
    virtual Coord ascent(long) const;
    virtual Coord descent(long) const;
    virtual Coord width(long) const;

    virtual Coord left_bearing(const char*, int) const;
    virtual Coord right_bearing(const char*, int) const;
    virtual Coord ascent(const char*, int) const;
    virtual Coord descent(const char*, int) const;
    virtual Coord width(const char*, int) const;

    virtual int index(const char*, int, float offset, boolean between) const;

    FontRep* rep(Display*) const;
protected:
    Font(FontImpl*);
private:
    friend class FontImpl;

    FontImpl* impl_;

    /* anachronisms */
public:
    int Baseline() const;
    boolean FixedWidth() const;
    int Height() const;
    int Index(const char*, int offset, boolean between) const;
    int Index(const char*, int, int offset, boolean between) const;
    int Width(const char*) const;
    int Width(const char*, int) const;
};

#include <InterViews/_leave.h>

#endif
