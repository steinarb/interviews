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

// $Header: drawing.c,v 1.19 90/01/11 22:11:12 dunwoody Exp $
// implements class Drawing.

#include "drawing.h"
#include "ipaint.h"
#include "istring.h"
#include "listboolean.h"
#include "listcenter.h"
#include "listgroup.h"
#include "listibrush.h"
#include "listicolor.h"
#include "listifont.h"
#include "listipattern.h"
#include "listselectn.h"
#include "page.h"
#include "slpict.h"
#include <InterViews/Graphic/polygons.h>
#include <InterViews/transformer.h>
#include <os/fs.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/file.h>			/* define constants for access call */

// Drawing creates the page, selection list, and clipboard filename.

Drawing::Drawing (double w, double h, double b) {
    const char* home = (home = getenv("HOME")) ? home : ".";
    const char* name = ".clipboard";
    clipfilename = new char[strlen(home) + 1 + strlen(name) + 1];
    strcpy(clipfilename, home);
    strcat(clipfilename, "/");
    strcat(clipfilename, name);
    page = new Page(w, h, b);
    picture = page->GetPicture();
    sl = new SelectionList;
}

// ~Drawing frees storage allocated for the clipboard filename, page,
// and selection list.

Drawing::~Drawing () {
    delete clipfilename;
    delete page;
    delete sl;
}

// Define access functions to return attributes.

boolean Drawing::GetLandscape () {
    Transformer* t = page->GetTransformer();
    return t ? t->Rotated90() : false;
}

Page* Drawing::GetPage () {
    return page;
}

void Drawing::GetPictureTT (Transformer& t) {
    picture->TotalTransformation(t);
}

SelectionList* Drawing::GetSelectionList () {
    return sl;
}

// Writable returns true only if the given drawing is writable.

boolean Drawing::Writable (const char* path) {
    return (access(path, W_OK) >= 0);
}

// Exists returns true only if the drawing already exists.

boolean Drawing::Exists (const char* path) {
    return (access(path, F_OK) >= 0);
}

// ClearPicture deletes the old picture and creates a new empty
// picture.

void Drawing::ClearPicture () {
    sl->DeleteAll();
    page->SetPicture(nil);
    picture = page->GetPicture();
}

// ReadPicture reads a new picture and replaces the old picture with
// the new picture if the read succeeds.

boolean Drawing::ReadPicture (const char* path, State* state) {
    boolean successful = false;
    if (path != nil) {
	FILE* stream = fopen(path, "r");
	if (stream != nil) {
	    PictSelection* newpic = new PictSelection(stream, state);
	    fclose(stream);
	    if (newpic->Valid()) {
		sl->DeleteAll();
		page->SetPicture(newpic);
		picture = page->GetPicture();
		successful = true;
	    } else {
		delete newpic;
		fprintf(stderr, "Drawing: input error in reading %s\n", path);
	    }
	}
    }
    return successful;
}

// PrintPicture prints the current picture by writing it through a
// pipe to a print command.

boolean Drawing::PrintPicture (const char* cmd, State* state) {
    boolean successful = false;
    if (cmd != nil) {
	FILE* stream = popen(cmd, "w");
	if (stream != nil) {
	    successful = picture->WritePicture(stream, state, true);
	    pclose(stream);
	}
    }
    return successful;
}

// WritePicture writes the current picture to a file.

boolean Drawing::WritePicture (const char* path, State* state) {
    boolean successful = false;
    if (path != nil) {
	FILE* stream = fopen(path, "w");
	if (stream != nil) {
	    successful = picture->WritePicture(stream, state, true);
	    fclose(stream);
	}
    }
    return successful;
}

// ReadClipboard returns copies of the Selections within the clipboard
// file in a newly allocated list.

SelectionList* Drawing::ReadClipboard (State* state) {
    SelectionList* sl = new SelectionList;
    FILE* stream = fopen(clipfilename, "r");
    if (stream != nil) {
	PictSelection* newpic = new PictSelection(stream, state);
	fclose(stream);
	if (newpic->Valid()) {
	    newpic->Propagate();
	    for (newpic->First(); !newpic->AtEnd(); newpic->RemoveCur()) {
		Selection* child = (Selection*) newpic->GetCurrent();
		sl->Append(new SelectionNode(child));
	    }
	}
	delete newpic;
    } else {
	fprintf(stderr, "Drawing: can't open %s\n", clipfilename);
    }
    return sl;
}

// WriteClipboard writes the picked Selections to the clipboard file,
// overwriting its previous contents.

void Drawing::WriteClipboard () {
    FILE* stream = fopen(clipfilename, "w");
    if (stream != nil) {
	PictSelection* newpic = new PictSelection;
	for (sl->First(); !sl->AtEnd(); sl->Next()) {
	    Graphic* copy = sl->GetCur()->GetSelection()->Copy();
	    newpic->Append(copy);
	}
	newpic->WritePicture(stream, nil, false);
	fclose(stream);
	delete newpic;
    } else {
	fprintf(stderr, "Drawing: can't open %s\n", clipfilename);
    }
}

// GetBox gets the smallest box bounding all the Selections.

void Drawing::GetBox (Coord& l, Coord& b, Coord& r, Coord& t) {
    BoxObj btotal;
    BoxObj bselection;

    if (sl->Size() >= 1) {
	sl->First()->GetSelection()->GetBox(btotal);
	for (sl->Next(); !sl->AtEnd(); sl->Next()) {
	    sl->GetCur()->GetSelection()->GetBox(bselection);
	    btotal = btotal + bselection;
	}
	l = btotal.left;
	b = btotal.bottom;
	r = btotal.right;
	t = btotal.top;
    }
}

// GetBrush returns the Selections' brush attributes in a newly
// allocated list.

IBrushList* Drawing::GetBrush () {
    IBrushList* brushlist = new IBrushList;
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	IBrush* brush = (IBrush*) sl->GetCur()->GetSelection()->GetBrush();
	brushlist->Append(new IBrushNode(brush));
    }
    return brushlist;
}

// GetCenter returns the Selections' centers in a newly allocated
// list.  It converts the centers from window coordinates to picture
// coordinates because only these coordinates will remain constant.

CenterList* Drawing::GetCenter () {
    CenterList* centerlist = new CenterList;
    Transformer t;
    picture->TotalTransformation(t);
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	float wincx, wincy, cx, cy;

	sl->GetCur()->GetSelection()->GetCenter(wincx, wincy);
	t.InvTransform(wincx, wincy, cx, cy);
	centerlist->Append(new CenterNode(cx, cy));
    }
    return centerlist;
}

// GetChildren returns the Selections and their children, if any, in a
// newly allocated list.

GroupList* Drawing::GetChildren () {
    GroupList* grouplist = new GroupList;
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	PictSelection* parent = (PictSelection*) sl->GetCur()->GetSelection();
	boolean haschildren = parent->HasChildren();
	SelectionList* children = new SelectionList;
	if (haschildren) {
	    for (parent->First(); !parent->AtEnd(); parent->Next()) {
		Selection* child = parent->GetCurrent();
		children->Append(new SelectionNode(child));
	    }
	}
	grouplist->Append(new GroupNode(parent, haschildren, children));
	delete children;
    }
    return grouplist;
}

// GetFgColor returns the Selections' FgColor attributes in a newly
// allocated list.

IColorList* Drawing::GetFgColor () {
    IColorList* fgcolorlist = new IColorList;
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	IColor* fgcolor = (IColor*) sl->GetCur()->GetSelection()->GetFgColor();
	fgcolorlist->Append(new IColorNode(fgcolor));
    }
    return fgcolorlist;
}

// GetBgColor returns the Selections' BgColor attributes in a newly
// allocated list.

IColorList* Drawing::GetBgColor () {
    IColorList* bgcolorlist = new IColorList;
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	IColor* bgcolor = (IColor*) sl->GetCur()->GetSelection()->GetBgColor();
	bgcolorlist->Append(new IColorNode(bgcolor));
    }
    return bgcolorlist;
}

// GetDuplicates duplicates the Selections, offsets them by one grid
// spacing, and returns them in a newly allocated list.

SelectionList* Drawing::GetDuplicates () {
    int offset = round(page->GetGridSpacing() * points);
    SelectionList* duplicates = new SelectionList;
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	Selection* dup = (Selection*) sl->GetCur()->GetSelection()->Copy();
	dup->Translate(offset, offset);
	duplicates->Append(new SelectionNode(dup));
    }
    return duplicates;
}

// GetFillBg returns the Selections' fillbg attributes in a newly
// allocated list.

booleanList* Drawing::GetFillBg () {
    booleanList* fillbglist = new booleanList;
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	boolean fillbg = sl->GetCur()->GetSelection()->BgFilled();
	fillbglist->Append(new booleanNode(fillbg)); 
    }
    return fillbglist;
}

// GetFont returns the Selections' Font attributes in a newly
// allocated list.

IFontList* Drawing::GetFont () {
    IFontList* fontlist = new IFontList;
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	IFont* font = (IFont*) sl->GetCur()->GetSelection()->GetFont();
	fontlist->Append(new IFontNode(font));
    }
    return fontlist;
}

// GetNumberOfGraphics returns the number of graphics in the
// Selections.

int Drawing::GetNumberOfGraphics () {
    int num = 0;
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	Selection* s = sl->GetCur()->GetSelection();
	if (s->HasChildren()) {
	    num += NumberOfGraphics((PictSelection*) s);
	} else {
	    ++num;
	}
    }
    return num;
}

// GetParent returns the Selections and their new parent in a newly
// allocated list if there are enough Selections to form a Group.

GroupList* Drawing::GetParent () {
    GroupList* grouplist = new GroupList;
    if (sl->Size() >= 2) {
	PictSelection* parent = new PictSelection;
	boolean haschildren = true;
	grouplist->Append(new GroupNode(parent, haschildren, sl));
    }
    return grouplist;
}

// GetPattern returns the Selections' Pattern attributes in a newly
// allocated list.

IPatternList* Drawing::GetPattern () {
    IPatternList* patternlist = new IPatternList;
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	IPattern* pattern =
	    (IPattern*) sl->GetCur()->GetSelection()->GetPattern();
	patternlist->Append(new IPatternNode(pattern));
    }
    return patternlist;
}

// GetPrevs returns the Selections' predecessors within the picture in
// a newly allocated list.

SelectionList* Drawing::GetPrevs () {
    SelectionList* prevlist = new SelectionList;
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	picture->SetCurrent(sl->GetCur()->GetSelection());
	Selection* prev = picture->Prev();
	prevlist->Append(new SelectionNode(prev));
    }
    return prevlist;
}

// GetSelections returns the Selections in a newly allocated list.

SelectionList* Drawing::GetSelections () {
    SelectionList* newsl = new SelectionList;
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	Selection* s = sl->GetCur()->GetSelection();
	newsl->Append(new SelectionNode(s));
    }
    return newsl;
}

// PickSelectionIntersecting returns the last Selection intersecting a
// box around the given point.

Selection* Drawing::PickSelectionIntersecting (Coord x, Coord y) {
    const int SLOP = 2;
    BoxObj pickpoint(x - SLOP, y - SLOP, x + SLOP, y + SLOP);
    return picture->LastSelectionIntersecting(pickpoint);
}

// PickSelectionShapedBy returns the last Selection shaped by a point
// close to the given point.

Selection* Drawing::PickSelectionShapedBy (Coord x, Coord y) {
    const float SLOP = 6.;
    for (picture->Last(); !picture->AtEnd(); picture->Prev()) {
	Selection* pick = picture->GetCurrent();
	if (pick->ShapedBy(x, y, SLOP)) {
	    return pick;
	}
    }
    return nil;
}

// PickSelectionsWithin returns all the Selections within the given
// box.

SelectionList* Drawing::PickSelectionsWithin (Coord l, Coord b, Coord r,
Coord t) {
    Selection** picks = nil;
    int numpicks = picture->SelectionsWithin(BoxObj(l, b, r, t), picks);
    SelectionList* picklist = new SelectionList;
    for (int i = 0; i < numpicks; i++) {
	picklist->Append(new SelectionNode(picks[i]));
    }
    delete picks;
    return picklist;
}

// Clear empties the SelectionList.

void Drawing::Clear () {
    sl->DeleteAll();
}

// Extend extends the SelectionList to include the picked Selection
// unless it's already there, in which case it removes the Selection.

void Drawing::Extend (Selection* pick) {
    if (!sl->Find(pick)) {
	sl->Append(new SelectionNode(pick));
    } else {
	sl->DeleteCur();
    }
}

// Extend extends the SelectionList to include the picked Selections
// unless they're already there, in which case it removes them.

void Drawing::Extend (SelectionList* picklist) {
    for (picklist->First(); !picklist->AtEnd(); picklist->Next()) {
	Selection* pick = picklist->GetCur()->GetSelection();
	Extend(pick);
    }
}

// Grasp selects the picked Selection only if the SelectionList does
// not already include it.

void Drawing::Grasp (Selection* pick) {
    if (!sl->Find(pick)) {
	Select(pick);
    }
}

// Select selects the picked Selection.

void Drawing::Select (Selection* pick) {
    sl->DeleteAll();
    sl->Append(new SelectionNode(pick));
}

// Select selects the picked Selections.

void Drawing::Select (SelectionList* picklist) {
    sl->DeleteAll();
    for (picklist->First(); !picklist->AtEnd(); picklist->Next()) {
	Selection* pick = picklist->GetCur()->GetSelection();
	sl->Append(new SelectionNode(pick));
    }
}

// SelectAll selects all of the Selections in the picture.

void Drawing::SelectAll () {
    sl->DeleteAll();
    for (picture->First(); !picture->AtEnd(); picture->Next()) {
	Selection* pick = picture->GetCurrent();
	sl->Append(new SelectionNode(pick));
    }
}

// Move translates the Selections.

void Drawing::Move (float xdisp, float ydisp) {
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	Selection* s = sl->GetCur()->GetSelection();
	s->Translate(xdisp, ydisp);
    }
}

// Scale scales the Selections about their centers.

void Drawing::Scale (float xscale, float yscale) {
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	Selection* s = sl->GetCur()->GetSelection();
	float cx, cy;
	s->GetCenter(cx, cy);
	s->Scale(xscale, yscale, cx, cy);
    }
}

// Stretch stretches the Selections while keeping the given side
// fixed.

void Drawing::Stretch (float stretch, Alignment side) {
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	Selection* s = sl->GetCur()->GetSelection();
	float l, b, r, t;
	s->GetBounds(l, b, r, t);
	switch (side) {
	case Left:
	    s->Scale(stretch, 1, r, t);
	    break;
	case Bottom:
	    s->Scale(1, stretch, r, t);
	    break;
	case Right:
	    s->Scale(stretch, 1, l, b);
	    break;
	case Top:
	    s->Scale(1, stretch, l, b);
	    break;
	default:
	    fprintf(stderr, "inappropriate enum passed to Drawing::Stretch\n");
	    break;
	}
    }
}

// Rotate rotates the Selections about their centers.

void Drawing::Rotate (float angle) {
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	Selection* s = sl->GetCur()->GetSelection();
	float cx, cy;
	s->GetCenter(cx, cy);
	s->Rotate(angle, cx, cy);
    }
}

// Align either aligns up all of the Selections or abuts all of them
// side to side, depending on whether the moving Selection's side or
// center aligns with the fixed Selection's same side or center.

void Drawing::Align (Alignment falign, Alignment malign) {
    if (falign == malign) {
	Selection* stays = sl->First()->GetSelection();
	for (sl->Next(); !sl->AtEnd(); sl->Next()) {
	    Selection* moves = sl->GetCur()->GetSelection();
	    stays->Align(falign, moves, malign);
	}
    } else {
	Selection* stays = sl->First()->GetSelection();
	for (sl->Next(); !sl->AtEnd(); sl->Next()) {
	    Selection* moves = sl->GetCur()->GetSelection();
	    stays->Align(falign, moves, malign);
	    stays = moves;
	}
    }
}

// AlignToGrid aligns the Selections' lower left corners to the
// nearest grid point.

void Drawing::AlignToGrid () {
    boolean gravity = page->GetGridGravity();
    page->SetGridGravity(true);
    Transformer t;
    picture->TotalTransformation(t);

    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	Selection* s = sl->GetCur()->GetSelection();
	float l, b, dummy;
	s->GetBounds(l, b, dummy, dummy);
	Coord nl = round(l);
	Coord nb = round(b);
	page->Constrain(nl, nb);
	float x0, y0, x1, y1;
	t.InvTransform(l, b, x0, y0);
	t.InvTransform(float(nl), float(nb), x1, y1);
	s->Translate(x1 - x0, y1 - y0);
    }

    page->SetGridGravity(gravity);
}

// SetBrush sets the Selections' brush attributes with the given brush
// attribute.

void Drawing::SetBrush (IBrush* brush) {
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	sl->GetCur()->GetSelection()->SetBrush(brush);
    }
}

// SetBrush sets each Selection's brush attribute with the
// corresponding brush attribute in the provided list.

void Drawing::SetBrush (IBrushList* brushlist) {
    for (sl->First(), brushlist->First(); !sl->AtEnd() && !brushlist->AtEnd();
	 sl->Next(), brushlist->Next())
    {
	IBrush* brush = brushlist->GetCur()->GetBrush();
	sl->GetCur()->GetSelection()->SetBrush(brush);
    }
}

// SetCenter centers each of the Selections over the corresponding
// position in the provided list.  It expects the passed postions to
// in picture coordinates, not window coordinates.

void Drawing::SetCenter (CenterList* centerlist) {
    Transformer t;
    picture->TotalTransformation(t);
    for (sl->First(), centerlist->First();
	 !sl->AtEnd() && !centerlist->AtEnd();
	 sl->Next(), centerlist->Next())
    {
	float winoldcx, winoldcy, oldcx, oldcy;
	float newcx = centerlist->GetCur()->GetCx();
	float newcy = centerlist->GetCur()->GetCy();
	Selection* s = sl->GetCur()->GetSelection();

	s->GetCenter(winoldcx, winoldcy);
	t.InvTransform(winoldcx, winoldcy, oldcx, oldcy);
	s->Translate(newcx - oldcx, newcy - oldcy);
    }
}

// SetFgColor sets the Selections' foreground color attributes with
// the given color attribute.

void Drawing::SetFgColor (IColor* fgcolor) {
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	Selection* s = sl->GetCur()->GetSelection();
	IColor* bgcolor = (IColor*) s->GetBgColor();
	s->SetColors(fgcolor, bgcolor);
    }
}

// SetFgColor sets the Selections' foreground color attributes with
// the corresponding color attributes in the provided list.

void Drawing::SetFgColor (IColorList* fgcolorlist) {
    for (sl->First(), fgcolorlist->First();
	 !sl->AtEnd() && !fgcolorlist->AtEnd();
	 sl->Next(), fgcolorlist->Next())
    {
	Selection* s = sl->GetCur()->GetSelection();
	IColor* fgcolor = fgcolorlist->GetCur()->GetColor();
	IColor* bgcolor = (IColor*) s->GetBgColor();
	s->SetColors(fgcolor, bgcolor);
    }
}

// SetBgColor sets the Selections' background color attributes with
// the given color attribute.

void Drawing::SetBgColor (IColor* bgcolor) {
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	Selection* s = sl->GetCur()->GetSelection();
	IColor* fgcolor = (IColor*) s->GetFgColor();
	s->SetColors(fgcolor, bgcolor);
    }
}

// SetBgColor sets the Selections' background color attributes with
// the corresponding color attributes in the provided list.

void Drawing::SetBgColor (IColorList* bgcolorlist) {
    for (sl->First(), bgcolorlist->First();
	 !sl->AtEnd() && !bgcolorlist->AtEnd();
	 sl->Next(), bgcolorlist->Next())
    {
	Selection* s = sl->GetCur()->GetSelection();
	IColor* fgcolor = (IColor*) s->GetFgColor();
	IColor* bgcolor = bgcolorlist->GetCur()->GetColor();
	s->SetColors(fgcolor, bgcolor);
    }
}

// SetFillBg sets the Selections' fillbg attributes with the given
// fillbg attribute.

void Drawing::SetFillBg (boolean fillbg) {
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	sl->GetCur()->GetSelection()->FillBg(fillbg);
    }
}

// SetFillBg sets each Selection's fillbg attribute with the
// corresponding fillbg attribute in the provided list.

void Drawing::SetFillBg (booleanList* fillbglist) {
    for (sl->First(), fillbglist->First();
	 !sl->AtEnd() && !fillbglist->AtEnd();
	 sl->Next(), fillbglist->Next())
    {
	boolean fillbg = fillbglist->GetCur()->GetBoolean();
	sl->GetCur()->GetSelection()->FillBg(fillbg);
    }
}

// SetFont sets the Selections' font attributes with the given font
// attribute.

void Drawing::SetFont (IFont* font) {
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	Selection* s = sl->GetCur()->GetSelection();
	s->SetFont(font);
    }
}

// SetFont sets each Selection's font attribute with the corresponding
// font attribute in the provided list.

void Drawing::SetFont (IFontList* fontlist) {
    for (sl->First(), fontlist->First(); !sl->AtEnd() && !fontlist->AtEnd();
	 sl->Next(), fontlist->Next())
    {
	IFont* font = fontlist->GetCur()->GetFont();
	Selection* s = sl->GetCur()->GetSelection();
	s->SetFont(font);
    }
}

// SetPattern sets the Selections' pattern attributes with the given
// pattern attribute.

void Drawing::SetPattern (IPattern* pattern) {
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	sl->GetCur()->GetSelection()->SetPattern(pattern);
    }
}

// SetPattern sets each Selection's pattern attribute with the
// corresponding pattern attribute in the provided list.

void Drawing::SetPattern (IPatternList* patternlist) {
    for (sl->First(), patternlist->First();
	 !sl->AtEnd() && !patternlist->AtEnd();
	 sl->Next(), patternlist->Next())
    {
	IPattern* pattern = patternlist->GetCur()->GetPattern();
	sl->GetCur()->GetSelection()->SetPattern(pattern);
    }
}

// Append appends the Selections to the picture.

void Drawing::Append () {
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	Selection* s = sl->GetCur()->GetSelection();
	picture->Append(s);
    }
}

// Group groups each parent's children, if any, under their parent and
// returns the resulting Selections in the SelectionList.

void Drawing::Group (GroupList* grouplist) {
    if (grouplist->Size() >= 1) {
	sl->DeleteAll();
	for (grouplist->First(); !grouplist->AtEnd(); grouplist->Next()) {
	    GroupNode* gn = grouplist->GetCur();
	    PictSelection* parent = gn->GetParent();
	    boolean haschildren = gn->GetHasChildren();
	    SelectionList* children = gn->GetChildren();
	    SelectionList* childrengs = gn->GetChildrenGS();
	    if (haschildren) {
		for (children->First(), childrengs->First();
		     !children->AtEnd() && !childrengs->AtEnd();
		     children->Next(), childrengs->Next())
		{
		    Graphic* child = children->GetCur()->GetSelection();
		    Graphic* childgs = childrengs->GetCur()->GetSelection();
		    *child = *childgs;
		    picture->SetCurrent(child);
		    picture->Remove(child);
		    parent->Append(child);
		}
		picture->InsertBeforeCur(parent);
	    }
	    sl->Append(new SelectionNode(parent));
	}
	Sort();
    }
}

// InsertAfterPrev inserts each Selection after its corresponding
// predecessor in the provided list.

void Drawing::InsertAfterPrev (SelectionList* prevlist) {
    for (sl->First(), prevlist->First(); !sl->AtEnd() && !prevlist->AtEnd();
	 sl->Next(), prevlist->Next())
    {
	Selection* prev = prevlist->GetCur()->GetSelection();
	picture->SetCurrent(prev);
	Selection* s = sl->GetCur()->GetSelection();
	picture->InsertAfterCur(s);
    }
}

// Prepend prepends the Selections to the picture.

void Drawing::Prepend () {
    for (sl->Last(); !sl->AtEnd(); sl->Prev()) {
	Selection* s = sl->GetCur()->GetSelection();
	picture->Prepend(s);
    }
}

// Remove removes the Selections from the picture.

void Drawing::Remove () {
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	Selection* s = sl->GetCur()->GetSelection();
	picture->Remove(s);
    }
}

// Replace replaces a Selection in the picture with a Selection not in it.

void Drawing::Replace (Selection* replacee, Selection* replacer) {
    picture->SetCurrent(replacee);
    picture->Remove(replacee);
    picture->InsertBeforeCur(replacer);
}

// Sort sorts the Selections so they occur in the same order as they
// do in the picture.

void Drawing::Sort () {
    if (sl->Size() >= 2) {
	for (picture->First(); !picture->AtEnd(); picture->Next()) {
	    Selection* g = picture->GetCurrent();
	    if (sl->Find(g)) {
		SelectionNode* s = sl->GetCur();
		sl->RemoveCur();
		sl->Append(s);
	    }
	}
    }
}

// Ungroup replaces all Selections which contain children with their
// children and returns the resulting Selections in the SelectionList.

void Drawing::Ungroup (GroupList* grouplist) {
    if (grouplist->Size() >= 1) {
	sl->DeleteAll();
	for (grouplist->First(); !grouplist->AtEnd(); grouplist->Next()) {
	    GroupNode* gn = grouplist->GetCur();
	    PictSelection* parent = gn->GetParent();
	    boolean haschildren = gn->GetHasChildren();
	    SelectionList* children = gn->GetChildren();
	    if (haschildren) {
		parent->Propagate();
		picture->SetCurrent(parent);
		for (children->First(); !children->AtEnd(); children->Next()) {
		    Selection* child = children->GetCur()->GetSelection();
		    parent->Remove(child);
		    picture->InsertBeforeCur(child);
		    sl->Append(new SelectionNode(child));
		}
		picture->Remove(parent);
	    } else {
		sl->Append(new SelectionNode(parent));
	    }
	}
	Sort();
    }
}

// NumberOfGraphics returns the number of graphics in the picture,
// calling itself recursively to count the number of graphics in
// subpictures.

int Drawing::NumberOfGraphics (PictSelection* picture) {
    int num = 0;
    for (picture->First(); !picture->AtEnd(); picture->Next()) {
	Selection* s = picture->GetCurrent();
	if (s->HasChildren()) {
	    num += NumberOfGraphics((PictSelection*) s);
	} else {
	    ++num;
	}
    }
    return num;
}
