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
 * Input cursors.
 */

#include <InterViews/color.h>
#include <InterViews/cursor.h>

static CursorPattern arrowPat = {
    0x0000, 0x4000, 0x6000, 0x7000, 0x7800, 0x7c00, 0x7e00, 0x7f00,
    0x7f80, 0x7c00, 0x6c00, 0x4600, 0x0600, 0x0300, 0x0300, 0x0000
};

static CursorPattern arrowMask = {
    0xc000, 0xe000, 0xf000, 0xf800, 0xfc00, 0xfe00, 0xff00, 0xff80,
    0xffc0, 0xffe0, 0xfe00, 0xef00, 0xcf00, 0x8780, 0x0780, 0x0380
};

static CursorPattern crossHairsPat = {
    0x0000, 0x0380, 0x0380, 0x0100, 0x0100, 0x0100, 0x600C, 0x7C7C, 
    0x600C, 0x0100, 0x0100, 0x0100, 0x0380, 0x0380, 0x0000, 0x0000, 
};

static CursorPattern crossHairsMask = {
    0x07C0, 0x07C0, 0x07C0, 0x0380, 0x0380, 0xE38E, 0xFC7E, 0xFC7E, 
    0xFC7E, 0xE38E, 0x0380, 0x0380, 0x07C0, 0x07C0, 0x07C0, 0x0000, 
};

static CursorPattern textPat = {
    0x0000, 0x4400, 0x2800, 0x1000, 0x1000, 0x1000, 0x1000, 0x1000, 
    0x1000, 0x1000, 0x1000, 0x1000, 0x1000, 0x2800, 0x4400, 0x0000
};

static CursorPattern textMask = {
    0x0000, 0xCC00, 0x7800, 0x3000, 0x3000, 0x3000, 0x3000, 0x3000, 
    0x3000, 0x3000, 0x3000, 0x3000, 0x3000, 0x7800, 0xCC00, 0x0000, 
};

static CursorPattern hourglassPat = {
    0x0000, 0x7FC0, 0x2080, 0x2080, 0x3180, 0x1F00, 0x0E00, 0x0400, 
    0x0400, 0x0E00, 0x1100, 0x2480, 0x2E80, 0x3F80, 0x7FC0, 0x0000, 
};

static CursorPattern hourglassMask = {
    0xFFE0, 0xFFE0, 0xFFE0, 0x7FC0, 0x7FC0, 0x3F80, 0x1F00, 0x0E00, 
    0x0E00, 0x1F00, 0x3F80, 0x7FC0, 0x7FC0, 0xFFE0, 0xFFE0, 0xFFE0
};

static CursorPattern ulPat = {
    0x0000, 0x7FE0, 0x7FE0, 0x6000, 0x6000, 0x6000, 0x6000, 0x6000,
    0x6000, 0x6000, 0x6000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

static CursorPattern ulMask = {
    0xFFF0, 0xFFF0, 0xFFF0, 0xFFF0, 0xF000, 0xF000, 0xF000, 0xF000,
    0xF000, 0xF000, 0xF000, 0xF000, 0x0000, 0x0000, 0x0000, 0x0000
};

static CursorPattern urPat = {
    0x0000, 0x07FE, 0x07FE, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
    0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

static CursorPattern urMask = {
    0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x000F, 0x000F, 0x000F, 0x000F,
    0x000F, 0x000F, 0x000F, 0x000F, 0x0000, 0x0000, 0x0000, 0x0000
};

static CursorPattern llPat = {
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6000, 0x6000, 0x6000,
    0x6000, 0x6000, 0x6000, 0x6000, 0x6000, 0x7fe0, 0x7fe0, 0x0000
};

static CursorPattern llMask = {
    0x0000, 0x0000, 0x0000, 0x0000, 0xF000, 0xF000, 0xF000, 0xF000,
    0xF000, 0xF000, 0xF000, 0xF000, 0xFFF0, 0xFFF0, 0xFFF0, 0xFFF0
};

static CursorPattern lrPat = {
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006,
    0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x07FE, 0x07FE, 0x0000
};

static CursorPattern lrMask = {
    0x0000, 0x0000, 0x0000, 0x0000, 0x000F, 0x000F, 0x000F, 0x000F,
    0x000F, 0x000F, 0x000F, 0x000F, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF
};

static CursorPattern noPat = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

Cursor* defaultCursor;
Cursor* arrow;
Cursor* crosshairs;
Cursor* ltextCursor;
Cursor* rtextCursor;
Cursor* hourglass;
Cursor* upperleft;
Cursor* upperright;
Cursor* lowerleft;
Cursor* lowerright;
Cursor* noCursor;

/*
 * Define the builtin cursors.
 */

void InitCursors (Color* fg, Color* bg) {
    arrow = new Cursor(1, 15, arrowPat, arrowMask, fg, bg);
    crosshairs = new Cursor(7, 8, crossHairsPat, crossHairsMask, fg, bg);
    ltextCursor = new Cursor(4, 8, textPat, textMask, fg, bg);
    rtextCursor = new Cursor(0, 8, textPat, textMask, fg, bg);
    hourglass = new Cursor(6, 8, hourglassPat, hourglassMask, fg, bg);
    upperleft = new Cursor(0, 15, ulPat, ulMask, fg, bg);
    upperright = new Cursor(15, 15, urPat, urMask, fg, bg);
    lowerleft = new Cursor(0, 0, llPat, llMask, fg, bg);
    lowerright = new Cursor(15, 0, lrPat, lrMask, fg, bg);
    noCursor = new Cursor(0, 0, noPat, noPat, fg, bg);
    defaultCursor = arrow;
}

Cursor::Cursor (
    short xoff, short yoff, CursorPattern p, CursorPattern m,
    Color* fg, Color* bg
) {
    id = nil;
    x = xoff;
    y = yoff;
    pat = p;
    mask = m;
    foreground = fg;
    background = bg;
}
