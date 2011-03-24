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
 * Interface to transformation matrices.
 */

#ifndef transformer_h
#define transformer_h

#include <InterViews/defs.h>
#include <InterViews/resource.h>

class Transformer : public Resource {
public:
    Transformer(Transformer* t =nil);	    // returns identity if t == nil
    Transformer(
	float a00, float a01, float a10, float a11, float a20, float a21
    );

    void GetEntries(
	float& a00, float& a01, float& a10, float& a11, float& a20, float& a21
    );
    void Premultiply(Transformer* t);
    void Postmultiply(Transformer* t);
    void Invert();

    void Translate(float dx, float dy);
    void Scale(float sx, float sy);
    void Rotate(float angle);
    boolean Translated (float = 1e-6);
    boolean Scaled (float = 1e-6);
    boolean Stretched (float = 1e-6);
    boolean Rotated (float = 1e-6);
    boolean Rotated90 (float = 1e-6);

    void Transform(Coord& x, Coord& y);
    void Transform(Coord x, Coord y, Coord& tx, Coord& ty);
    void Transform(float x, float y, float& tx, float& ty);
    void TransformList(Coord x[], Coord y[], int n);
    void TransformList(Coord x[], Coord y[], int n, Coord tx[], Coord ty[]);
    void TransformRect(Coord&, Coord&, Coord&, Coord&);
    void TransformRect(float&, float&, float&, float&);
    void InvTransform(Coord& tx, Coord& ty);
    void InvTransform(Coord tx, Coord ty, Coord& x, Coord& y);
    void InvTransform(float tx, float ty, float& x, float& y);
    void InvTransformList(Coord tx[], Coord ty[], int n);
    void InvTransformList(Coord tx[], Coord ty[], int n, Coord x[], Coord y[]);
    void InvTransformRect(Coord&, Coord&, Coord&, Coord&);
    void InvTransformRect(float&, float&, float&, float&);

    boolean operator == (Transformer&);
    boolean operator != (Transformer&);
    Transformer& operator = (Transformer&);
private:
    float Det(Transformer*);
private:
    float mat00, mat01, mat10, mat11, mat20, mat21;
};

inline float Transformer::Det (Transformer *t) {
    return t->mat00*t->mat11 - t->mat01*t->mat10;
}

inline boolean Transformer::Translated (float tol) {
    return -tol > mat20 || mat20 > tol || -tol > mat21 || mat21 > tol;
}

inline boolean Transformer::Scaled (float tol) {
    float l = 1 - tol, u = 1 + tol;

    return l > mat00 || mat00 > u || l > mat11 || mat11 > u;
}

inline boolean Transformer::Stretched (float tol) {
    float diff = mat00 - mat11;

    return -tol > diff || diff > tol;
}

inline boolean Transformer::Rotated (float tol) {
    return -tol > mat01 || mat01 > tol || -tol > mat10 || mat10 > tol;
}

inline boolean Transformer::Rotated90 (float tol) {
    return Rotated(tol) && -tol <= mat00 && mat00 <= tol && 
        -tol <= mat11 && mat11 <= tol;
}

#endif
