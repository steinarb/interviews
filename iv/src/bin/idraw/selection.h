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

// $Header: selection.h,v 1.14 89/10/09 14:49:24 linton Exp $
// declares classes Selection and NPtSelection.

#ifndef selection_h
#define selection_h

#include <InterViews/Graphic/picture.h>

// Declare imported types.

class Rubberband;
class RubberVertex;
class State;
class istream;
class ostream;

// PostScript file format changes.

static const int ORIGINALVERSION = 1;	  // original format
static const int FGCOLORVERSION = 2;	  // added foreground color
static const int NONREDUNDANTVERSION = 3; // eliminated unnecessary text
					  // pattern and duplication of
					  // font name, transformation matrix,
					  // poly points, and text data
static const int FGANDBGCOLORVERSION = 4; // added background color and
					  // RGB values for overriding names;
					  // used graylevel to eliminate
					  // redundant patternfill data
static const int GRIDSPACINGVERSION = 5;  // added grid spacing
static const int NONROTATEDVERSION = 6;	  // replaced rotation of drawing with
					  // rotation of view for landscape
static const int TEXTOFFSETVERSION = 7;	  // changed text positions on screen
					  // and improved accuracy of
					  // text positions on printout

// Other constants.

static const int ARROWHEIGHT = 8;      // how long arrows are in points
static const int ARROWWIDTH = 4;       // how wide arrows are in points
static const int BUFSIZE = 256;	       // size of buffer for reading data
static const int HDSIZE = 5;	       // how wide handles are in points

// A Selection can draw handles around itself and create a reshaped
// copy of itself.

extern const char* startdata;	       // signals place to read valid data

class Selection : public Picture {
public:

    Selection(Graphic* = nil);
    ~Selection();

    Graphic* Copy();
    boolean HasChildren();

    void GetPaddedBox(BoxObj&);
    void DrawHandles(Painter*, Canvas*);
    void EraseHandles(Painter*, Canvas*);
    void RedrawHandles(Painter*, Canvas*);
    void RedrawUnclippedHandles(Painter*, Canvas*);
    void ResetHandles();

    virtual boolean ShapedBy(Coord, Coord, float);
    virtual Rubberband* CreateShape(Coord, Coord);
    virtual Selection* GetReshapedCopy();

    virtual void WriteData(ostream&);

protected:

    void Skip(istream&);
    void ReadVersion(istream&);
    void ReadGridSpacing(istream&, State*);
    void ReadGS(istream&, State*);
    void ReadPictGS(istream&, State*);
    void ReadTextGS(istream&, State*);
    void ReadBrush(istream&, State*);
    void ReadFgColor(istream&, State*);
    void ReadBgColor(istream&, State*);
    void ReadFont(istream&, State*);
    void ReadPattern(istream&, State*);
    void ReadTransformer(istream&);
    float CalcGrayLevel(int);

    void WriteVersion(ostream&);
    void WriteGridSpacing(ostream&, State*);
    void WriteGS(ostream&);
    void WritePictGS(ostream&);
    void WriteTextGS(ostream&);
    void WriteBrush(ostream&);
    void WriteFgColor(ostream&);
    void WriteBgColor(ostream&);
    void WriteFont(ostream&);
    void WritePattern(ostream&);
    void WriteTransformer(ostream&);

    virtual void CreateHandles();
    void DeleteHandles();

    Rubberband* handles;	// contains handles outlining Selection

    static char buf[BUFSIZE];	// contains storage for reading data
    static int versionnumber;	// stores version of drawing read from file

};

// An NPtSelection knows how to create handles, read or write its
// points, and draw arrowheads so many subclasses can reuse the same
// code.

class NPtSelection : public Selection {
public:

    NPtSelection(Graphic*);

    virtual int GetOriginal(const Coord*&, const Coord*&);
    boolean ShapedBy(Coord, Coord, float);
    Rubberband* CreateShape(Coord, Coord);
    Selection* GetReshapedCopy();

protected:

    void ReadPoints(istream&, const Coord*&, const Coord*&, int&);
    void WriteData(ostream&);

    virtual RubberVertex* CreateRubberVertex(Coord*, Coord*, int, int);
    virtual Selection* CreateReshapedCopy(Coord*, Coord*, int);
    void CreateHandles();
    void TotalTransform(Coord*, Coord*, int);
    void InvTotalTransform(Coord*, Coord*, int);
    int ClosestPoint(Coord*, Coord*, int, Coord, Coord);

    boolean LeftAcont(Coord, Coord, Coord, Coord, PointObj&, Graphic*);
    boolean RightAcont(Coord, Coord, Coord, Coord, PointObj&, Graphic*);
    boolean LeftAints(Coord, Coord, Coord, Coord, BoxObj&, Graphic*);
    boolean RightAints(Coord, Coord, Coord, Coord, BoxObj&, Graphic*);
    void drawLeftA(Coord, Coord, Coord, Coord, Canvas*, Graphic*);
    void drawRightA(Coord, Coord, Coord, Coord, Canvas*, Graphic*);
    float MergeArrowHeadTol(float, Graphic*);
    boolean ArrowHeadcont(Coord, Coord, Coord, Coord, PointObj&, Graphic*);
    boolean ArrowHeadints(Coord, Coord, Coord, Coord, BoxObj&, Graphic*);
    void drawArrowHead(Coord, Coord, Coord, Coord, Canvas*, Graphic*);
    void SetCTM(Coord, Coord, Coord, Coord, Graphic*, boolean);
    void RestoreCTM(Graphic*);
    float Slope(float, float);

    const char* myname;		// tells which NPtSelection subclass this is
    RubberVertex* rubbervertex;	// tells us how the user wants to reshape us

};

#endif
