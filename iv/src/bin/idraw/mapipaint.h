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

// $Header: mapipaint.h,v 1.7 89/10/09 14:49:01 linton Exp $
// declares MapIPaint subclasses.

#ifndef mapipaint_h
#define mapipaint_h

#include <InterViews/defs.h>

// Declare imported types.

class BaseList;
class BaseNode;
class IBrush;
class IBrushList;
class IColor;
class IColorList;
class IFont;
class IFontList;
class IPattern;
class IPatternList;
class Interactor;

// A MapIPaint creates a list of predefined and user-defined entries.

class MapIPaint {
protected:

    void Init(BaseList*, Interactor*, const char*);
    void DefineEntries(BaseList*, Interactor*, const char*);
    void DefineInitial(Interactor*, const char*);
    virtual BaseNode* CreateEntry(const char*);

    int initial;		// denotes which entry is used on startup

};

// A MapIBrush manages a list of predefined and user-defined brushes.

class MapIBrush : public MapIPaint {
public:

    MapIBrush(Interactor*, const char*);
    ~MapIBrush();

    int Size();
    boolean AtEnd();
    IBrush* First();
    IBrush* Last();
    IBrush* Prev();
    IBrush* Next();
    IBrush* GetCur();
    IBrush* Index(int);
    boolean Find(IBrush*);
    IBrush* GetInitial();

    IBrush* FindOrAppend(boolean, int, int, int, int);

protected:

    BaseNode* CreateEntry(const char*);

    IBrushList* ibrushlist;	    // stores idraw's IBrushes

};

// A MapIColor manages a list of predefined and user-defined colors.

class MapIColor : public MapIPaint {
public:

    MapIColor(Interactor*, const char*);
    ~MapIColor();

    int Size();
    boolean AtEnd();
    IColor* First();
    IColor* Last();
    IColor* Prev();
    IColor* Next();
    IColor* GetCur();
    IColor* Index(int);
    boolean Find(IColor*);
    IColor* GetInitial();

    IColor* FindOrAppend(const char*, int, int, int);

protected:

    BaseNode* CreateEntry(const char*);

    IColorList* icolorlist;	    // stores idraw's IColors

};

// A MapIFont manages a list of predefined and user-defined fonts.

class MapIFont : public MapIPaint {
public:

    MapIFont(Interactor*, const char*);
    ~MapIFont();

    int Size();
    boolean AtEnd();
    IFont* First();
    IFont* Last();
    IFont* Prev();
    IFont* Next();
    IFont* GetCur();
    IFont* Index(int);
    boolean Find(IFont*);
    IFont* GetInitial();

    IFont* FindOrAppend(const char*, const char*, const char*);

protected:

    BaseNode* CreateEntry(const char*);

    IFontList* ifontlist;	    // stores idraw's IFonts

};

// A MapIPattern manages a list of predefined and user-defined
// patterns.

class MapIPattern : public MapIPaint {
public:

    MapIPattern(Interactor*, const char*);
    ~MapIPattern();

    int Size();
    boolean AtEnd();
    IPattern* First();
    IPattern* Last();
    IPattern* Prev();
    IPattern* Next();
    IPattern* GetCur();
    IPattern* Index(int);
    boolean Find(IPattern*);
    IPattern* GetInitial();

    IPattern* FindOrAppend(boolean, float, int*, int);

protected:

    BaseNode* CreateEntry(const char*);
    int CalcBitmap(float);

    void ExpandToFullSize(int*, int);

    IPatternList* ipatternlist;	    // stores idraw's IPatterns

};

#endif
