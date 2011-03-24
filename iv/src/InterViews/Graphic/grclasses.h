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
 * Unique Graphic object class identifiers
 * (must be in range 2000 <= ClassId <= 2999)
 */

#ifndef grclasses_h
#define grclasses_h

static const int GRAPHIC	= 2004;
static const int FULL_GRAPHIC	= 2005;
static const int PICTURE	= 2006;
static const int INSTANCE	= 2007;
static const int ELLIPSE	= 2008;
static const int FILLELLIPSE	= 2009;
static const int CIRCLE		= 2010;
static const int FILLCIRCLE	= 2011;
static const int POINT		= 2012;
static const int LINE		= 2013;
static const int MULTILINE	= 2014;
static const int RECT		= 2015;
static const int FILLRECT	= 2016;
static const int POLYGON	= 2017;
static const int FILLPOLYGON	= 2018;
static const int BSPLINE	= 2019;
static const int CLOSEDBSPLINE	= 2020;
static const int FILLBSPLINE	= 2021;
static const int LABEL		= 2022;
static const int PCOLOR		= 2023;
static const int POINTOBJ	= 2025;
static const int LINEOBJ	= 2026;
static const int BOXOBJ		= 2027;
static const int MULTILINEOBJ	= 2028;
static const int FILLPOLYGONOBJ	= 2029;
static const int PPATTERN	= 2033;
static const int PFONT		= 2034;
static const int PBRUSH		= 2035;
static const int RASTERRECT     = 2036;
static const int STENCIL        = 2037;

#endif
