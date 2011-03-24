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

static const unsigned GRAPHIC		= 2004;
static const unsigned FULL_GRAPHIC	= 2005;
static const unsigned PICTURE		= 2006;
static const unsigned INSTANCE		= 2007;
static const unsigned ELLIPSE		= 2008;
static const unsigned FILLELLIPSE	= 2009;
static const unsigned CIRCLE		= 2010;
static const unsigned FILLCIRCLE	= 2011;
static const unsigned POINT		= 2012;
static const unsigned LINE		= 2013;
static const unsigned MULTILINE		= 2014;
static const unsigned RECT		= 2015;
static const unsigned FILLRECT		= 2016;
static const unsigned POLYGON		= 2017;
static const unsigned FILLPOLYGON	= 2018;
static const unsigned BSPLINE		= 2019;
static const unsigned CLOSEDBSPLINE	= 2020;
static const unsigned FILLBSPLINE	= 2021;
static const unsigned LABEL		= 2022;
static const unsigned PCOLOR		= 2023;
static const unsigned POINTOBJ		= 2025;
static const unsigned LINEOBJ		= 2026;
static const unsigned BOXOBJ		= 2027;
static const unsigned MULTILINEOBJ	= 2028;
static const unsigned FILLPOLYGONOBJ	= 2029;
static const unsigned PPATTERN		= 2033;
static const unsigned PFONT		= 2034;
static const unsigned PBRUSH		= 2035;
static const unsigned RASTERRECT	= 2036;
static const unsigned STENCIL		= 2037;

#endif
