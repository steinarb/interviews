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
 * Implementation of transformation matrix operations.
 */

#include <InterViews/transformer.h>
#include <math.h>

static const float RADPERDEG = M_PI/180.0;

Transformer::Transformer (Transformer* t) {
    if (t == nil) {
	mat00 = mat11 = 1;			// identity
	mat01 = mat10 = mat20 = mat21 = 0;
    } else {
	mat00 = t->mat00;	mat01 = t->mat01;
	mat10 = t->mat10;	mat11 = t->mat11;
	mat20 = t->mat20;	mat21 = t->mat21;
    }
    Reference();
}

Transformer::Transformer (
    float a00, float a01, float a10, float a11, float a20, float a21
) {
    mat00 = a00;	mat01 = a01;
    mat10 = a10;	mat11 = a11;
    mat20 = a20;	mat21 = a21;
    Reference();
}

void Transformer::GetEntries (
    float& a00, float& a01, float& a10, float& a11, float& a20, float& a21
) {
    a00 = mat00;
    a01 = mat01;
    a10 = mat10;
    a11 = mat11;
    a20 = mat20;
    a21 = mat21;
}

void Transformer::Translate (float dx, float dy) {
    mat20 += dx;
    mat21 += dy;
}

void Transformer::Scale (float sx, float sy) {
    mat00 *= sx;
    mat01 *= sy;
    mat10 *= sx;
    mat11 *= sy;
    mat20 *= sx;
    mat21 *= sy;
}

void Transformer::Rotate (float angle) {
    float tmp1, tmp2, m00, m01, m10, m11, m20, m21;

    angle *= RADPERDEG;
    tmp1 = cos(angle);
    tmp2 = sin(angle);
    
    m00 = mat00*tmp1;
    m01 = mat01*tmp2;
    m10 = mat10*tmp1;
    m11 = mat11*tmp2;
    m20 = mat20*tmp1;
    m21 = mat21*tmp2;

    mat01 = mat00*tmp2 + mat01*tmp1;
    mat11 = mat10*tmp2 + mat11*tmp1;
    mat21 = mat20*tmp2 + mat21*tmp1;
    mat00 = m00 - m01;
    mat10 = m10 - m11;
    mat20 = m20 - m21;
}

void Transformer::Premultiply (Transformer* t) {
    float tmp1 = mat00;
    float tmp2 = mat10;

    mat00  = t->mat00*tmp1 + t->mat01*tmp2;
    mat10  = t->mat10*tmp1 + t->mat11*tmp2;
    mat20 += t->mat20*tmp1 + t->mat21*tmp2;

    tmp1 = mat01;
    tmp2 = mat11;

    mat01  = t->mat00*tmp1 + t->mat01*tmp2;
    mat11  = t->mat10*tmp1 + t->mat11*tmp2;
    mat21 += t->mat20*tmp1 + t->mat21*tmp2;
}

void Transformer::Postmultiply (Transformer* t) {
    float tmp = mat00*t->mat01 + mat01*t->mat11;
    mat00 = mat00*t->mat00 + mat01*t->mat10;
    mat01 = tmp;

    tmp = mat10*t->mat01 + mat11*t->mat11;
    mat10 = mat10*t->mat00 + mat11*t->mat10;
    mat11 = tmp;

    tmp = mat20*t->mat01 + mat21*t->mat11;
    mat20 = mat20*t->mat00 + mat21*t->mat10;
    mat21 = tmp;

    mat20 += t->mat20;
    mat21 += t->mat21;
}    

void Transformer::Invert () {
    float d = Det(this);
    float t00 = mat00;
    float t20 = mat20;

    mat20 = (mat10*mat21 - mat11*mat20)/d;
    mat21 = (mat01*t20 - mat00*mat21)/d;
    mat00 = mat11/d;
    mat11 = t00/d;
    mat10 = -mat10/d;
    mat01 = -mat01/d;
}

void Transformer::Transform (Coord& x, Coord& y) {
    Coord tx = x;
    x = round(tx*mat00 + y*mat10 + mat20);
    y = round(tx*mat01 + y*mat11 + mat21);
}

void Transformer::InvTransform (Coord& tx, Coord& ty) {
    float d = Det(this);
    float a = (float(tx) - mat20) / d;
    float b = (float(ty) - mat21) / d;

    tx = round(a*mat11 - b*mat10);
    ty = round(b*mat00 - a*mat01);
}

void Transformer::InvTransform (Coord tx, Coord ty, Coord& x, Coord& y) {
    float d = Det(this);
    float a = (float(tx) - mat20) / d;
    float b = (float(ty) - mat21) / d;

    x = round(a*mat11 - b*mat10);
    y = round(b*mat00 - a*mat01);
}

void Transformer::InvTransform (float tx, float ty, float& x, float& y) {
    float d = Det(this);
    float a = (tx - mat20) / d;
    float b = (ty - mat21) / d;

    x = a*mat11 - b*mat10;
    y = b*mat00 - a*mat01;
}

void Transformer::TransformList (Coord x[], Coord y[], int n) {
    register Coord* ox, * oy;
    Coord* lim;

    lim = &x[n];
    for (ox = x, oy = y; ox < lim; ox++, oy++) {
	Transform(*ox, *oy);
    }
}

void Transformer::TransformList (
    Coord x[], Coord y[], int n, Coord tx[], Coord ty[]
) {
    register Coord* ox, * oy, * nx, * ny;
    Coord* lim;

    lim = &x[n];
    for (ox = x, oy = y, nx = tx, ny = ty; ox < lim; ox++, oy++, nx++, ny++) {
	Transform(*ox, *oy, *nx, *ny);
    }
}

void Transformer::InvTransformList (Coord tx[], Coord ty[], int n) {
    register Coord* ox, * oy;
    Coord* lim;
    float a, b, d = Det(this);

    lim = &tx[n];
    for (ox = tx, oy = ty; ox < lim; ox++, oy++) {
        a = (float(*ox) - mat20) / d;
        b = (float(*oy) - mat21) / d;

        *ox = round(a*mat11 - b*mat10);
        *oy = round(b*mat00 - a*mat01);
    }
}

void Transformer::InvTransformList (
    Coord tx[], Coord ty[], int n, Coord x[], Coord y[]
) {
    register Coord* ox, * oy, * nx, * ny;
    Coord* lim;
    float a, b, d = Det(this);

    lim = &tx[n];
    for (ox = tx, oy = ty, nx = x, ny = y; ox < lim; ox++, oy++, nx++, ny++) {
        a = (float(*ox) - mat20) / d;
        b = (float(*oy) - mat21) / d;

        *nx = round(a*mat11 - b*mat10);
        *ny = round(b*mat00 - a*mat01);
    }
}

void Transformer::TransformRect (Coord& x0, Coord& y0, Coord& x1, Coord& y1) {
    float tx00, ty00, tx10, ty10, tx11, ty11, tx01, ty01;

    Transform(float(x0), float(y0), tx00, ty00);
    Transform(float(x1), float(y0), tx10, ty10);
    Transform(float(x1), float(y1), tx11, ty11);
    Transform(float(x0), float(y1), tx01, ty01);
    x0 = round(min(tx00, min(tx01, min(tx10, tx11))));
    y0 = round(min(ty00, min(ty01, min(ty10, ty11))));
    x1 = round(max(tx00, max(tx01, max(tx10, tx11))));
    y1 = round(max(ty00, max(ty01, max(ty10, ty11))));
}

void Transformer::TransformRect (float& x0, float& y0, float& x1, float& y1) {
    float tx00, ty00, tx10, ty10, tx11, ty11, tx01, ty01;

    Transform(x0, y0, tx00, ty00);
    Transform(x1, y0, tx10, ty10);
    Transform(x1, y1, tx11, ty11);
    Transform(x0, y1, tx01, ty01);
    x0 = min(tx00, min(tx01, min(tx10, tx11)));
    y0 = min(ty00, min(ty01, min(ty10, ty11)));
    x1 = max(tx00, max(tx01, max(tx10, tx11)));
    y1 = max(ty00, max(ty01, max(ty10, ty11)));
}

void Transformer::InvTransformRect (
    Coord& x0, Coord& y0, Coord& x1, Coord& y1
) {
    float tx00, ty00, tx10, ty10, tx11, ty11, tx01, ty01;

    InvTransform(float(x0), float(y0), tx00, ty00);
    InvTransform(float(x1), float(y0), tx10, ty10);
    InvTransform(float(x1), float(y1), tx11, ty11);
    InvTransform(float(x0), float(y1), tx01, ty01);
    x0 = round(min(tx00, min(tx01, min(tx10, tx11))));
    y0 = round(min(ty00, min(ty01, min(ty10, ty11))));
    x1 = round(max(tx00, max(tx01, max(tx10, tx11))));
    y1 = round(max(ty00, max(ty01, max(ty10, ty11))));
}

void Transformer::InvTransformRect (
    float& x0, float& y0, float& x1, float& y1
) {
    float tx00, ty00, tx10, ty10, tx11, ty11, tx01, ty01;

    InvTransform(x0, y0, tx00, ty00);
    InvTransform(x1, y0, tx10, ty10);
    InvTransform(x1, y1, tx11, ty11);
    InvTransform(x0, y1, tx01, ty01);
    x0 = min(tx00, min(tx01, min(tx10, tx11)));
    y0 = min(ty00, min(ty01, min(ty10, ty11)));
    x1 = max(tx00, max(tx01, max(tx10, tx11)));
    y1 = max(ty00, max(ty01, max(ty10, ty11)));
}

boolean Transformer::operator == (Transformer& t) {
    return
	mat00 == t.mat00 && mat01 == t.mat01 &&
	mat10 == t.mat10 && mat11 == t.mat11 &&
	mat20 == t.mat20 && mat21 == t.mat21;
}

boolean Transformer::operator != (Transformer& t) {
    return
	mat00 != t.mat00 || mat01 != t.mat01 ||
	mat10 != t.mat10 || mat11 != t.mat11 ||
	mat20 != t.mat20 || mat21 != t.mat21;
}

Transformer& Transformer::operator = (Transformer& t) {
    mat00 = t.mat00;
    mat01 = t.mat01;
    mat10 = t.mat10;
    mat11 = t.mat11;
    mat20 = t.mat20;
    mat21 = t.mat21;
    return *this;
}

void Transformer::Transform (Coord x, Coord y, Coord& tx, Coord& ty) {
    tx = round(x*mat00 + y*mat10 + mat20);
    ty = round(x*mat01 + y*mat11 + mat21);
}

void Transformer::Transform (float x, float y, float& tx, float& ty) {
    tx = x*mat00 + y*mat10 + mat20;
    ty = x*mat01 + y*mat11 + mat21;
}
