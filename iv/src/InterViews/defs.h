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
 * Common definitions for all of InterViews.
 */

#ifndef iv_defs_h
#define iv_defs_h

/*
 * A boolean type can't be an enum because C++ enums should not be
 * promoted to ints.  So, boolean f() { return !g(); } is an error
 * if boolean is an enum.
 */

typedef unsigned boolean;

static const boolean false = 0;
static const boolean true = 1;

/*
 * Alignment needs to be unsigned so that it can be stored
 * as a bit field.
 */

typedef unsigned Alignment;

static const Alignment TopLeft = 0;
static const Alignment TopCenter = 1;
static const Alignment TopRight = 2;
static const Alignment CenterLeft = 3;
static const Alignment Center = 4;
static const Alignment CenterRight = 5;
static const Alignment BottomLeft = 6;
static const Alignment BottomCenter = 7;
static const Alignment BottomRight = 8;
static const Alignment Left = 9;
static const Alignment Right = 10;
static const Alignment Top = 11;
static const Alignment Bottom = 12;
static const Alignment HorizCenter = 13;
static const Alignment VertCenter = 14;

enum TextStyle {
    Plain = 0x0,
    Boldface = 0x1,
    Underlined = 0x2,
    Reversed = 0x4,
    Outlined = 0x8
};

typedef int Coord;

extern double inch, inches, cm, point, points;
static const int pixels = 1;

#ifndef nil
#define nil 0
#endif

#if !defined(__cplusplus)
overload min;
overload max;
#endif

inline int min (int a, int b) { return a < b ? a : b; }
inline float min (float a, float b) { return a < b ? a : b; }
inline double min (double a, double b) { return a < b ? a : b; }

inline int max (int a, int b) { return a > b ? a : b; }
inline float max (float a, float b) { return a > b ? a : b; }
inline double max (double a, double b) { return a > b ? a : b; }

inline int round (double x) { return x > 0 ? int(x+0.5) : -int(-x+0.5); }

#endif
