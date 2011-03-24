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
 * An input cursor is defined by two 16x16 bitmaps, one that
 * specifies which pixels are to be drawn and one that specifies
 * which pixels are in foreground color and which in background color.
 * If a device does not support a mask the background pixels are not drawn.
 */

#ifndef cursor_h
#define cursor_h

static const int cursorHeight = 16;
static const int cursorWidth = 16;

typedef int CursorPattern[cursorHeight];

class Color;
class Bitmap;
class Font;

class Cursor {
public:
    Cursor(
	short xoff, short yoff,			/* hot spot */
	CursorPattern pat, CursorPattern mask,	/* pattern, mask */
	Color* fg, Color* bg
    );
    Cursor(Bitmap* pat, Bitmap* mask, Color* fg, Color* bg);
    Cursor(Font*, int pat, int mask, Color* fg, Color* bg);
    Cursor(int, Color* fg, Color* bg);
    ~Cursor();

    void* Id();
private:
    void* id;
    short x, y;
    int* pat, * mask;
    Color* foreground, * background;
};

/*
 * Predefined cursors.
 */

extern Cursor* defaultCursor;
extern Cursor* arrow;
extern Cursor* crosshairs;
extern Cursor* ltextCursor;
extern Cursor* rtextCursor;
extern Cursor* hourglass;
extern Cursor* upperleft;
extern Cursor* upperright;
extern Cursor* lowerleft;
extern Cursor* lowerright;
extern Cursor* noCursor;

#endif
