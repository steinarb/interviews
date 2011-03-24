/*
 * Copyright (c) 1987, 1988, 1989, 1990, 1991 Stanford University
 * Copyright (c) 1991 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and 
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Stanford and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Stanford and Silicon Graphics.
 * 
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 *
 * IN NO EVENT SHALL STANFORD OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
 * OF THIS SOFTWARE.
 */

#ifndef iv_geometry_h
#define iv_geometry_h

#include <InterViews/boolean.h>
#include <InterViews/coord.h>
#include <OS/math.h>

#include <InterViews/_enter.h>

enum DimensionName {
    Dimension_X, Dimension_Y, Dimension_Z, Dimension_Undefined
};

class Requirement {
public:
    Requirement();
    Requirement(Coord natural);
    Requirement(Coord natural, Coord stretch, Coord shrink, float alignment);
    Requirement(
        Coord natural_lead, Coord max_lead, Coord min_lead,
        Coord natural_trail, Coord max_trail, Coord min_trail
    );

    boolean equals(const Requirement&, float epsilon) const;
    boolean defined() const;

    void natural(Coord);
    Coord natural() const;

    void stretch(Coord);
    Coord stretch() const;

    void shrink(Coord);
    Coord shrink() const;

    void alignment(float);
    float alignment() const;
private:
    Coord natural_;
    Coord stretch_;
    Coord shrink_;
    float alignment_;
};

class Requisition {
public:
    Requisition();
    Requisition(const Requisition&);

    void penalty(int);
    int penalty() const;

    boolean equals(const Requisition&, float epsilon) const;

    void require(DimensionName, const Requirement&);
    const Requirement& requirement(DimensionName) const;
    Requirement& requirement(DimensionName);
private:
    int penalty_;
    Requirement x_;
    Requirement y_;
};

class Allotment {
public:
    Allotment();
    Allotment(Coord origin, Coord span, float alignment);

    boolean equals(const Allotment&, float epsilon) const;

    void origin(Coord);
    void offset(Coord);
    Coord origin() const;
    void span(Coord);
    Coord span() const;

    void alignment(float);
    float alignment() const;

    Coord begin() const;
    Coord end() const;
private:
    Coord origin_;
    Coord span_;
    float alignment_;
};

class Allocation {
public:
    Allocation();
    Allocation(const Allocation&);

    boolean equals(const Allocation&, float epsilon) const;

    void allot(DimensionName, const Allotment&);
    void allot_x(const Allotment&);
    void allot_y(const Allotment&);
    Allotment& allotment(DimensionName);
    const Allotment& allotment(DimensionName) const;
    Allotment& x_allotment();
    Allotment& y_allotment();
    const Allotment& x_allotment() const;
    const Allotment& y_allotment() const;

    Coord x() const;
    Coord y() const;
    Coord left() const;
    Coord right() const;
    Coord bottom() const;
    Coord top() const;
private:
    Allotment x_;
    Allotment y_;
};

class Extension {
public:
    Extension();
    Extension(const Extension&);

    void extend(const Extension&);
    void extend(const Allocation&);

    void extent(DimensionName, Coord begin, Coord end);
    void get_extent(DimensionName, Coord& begin, Coord& end) const;

    void xy_extents(Coord x_begin, Coord x_end, Coord y_begin, Coord y_end);
    void xy_extents(const Allocation&);

    Coord left() const;
    Coord bottom() const;
    Coord right() const;
    Coord top() const;
private:
    Coord x_begin_;
    Coord x_end_;
    Coord y_begin_;
    Coord y_end_;
};

inline Requirement::Requirement() {
    natural_ = -fil;
    stretch_ = 0;
    shrink_ = 0;
    alignment_ = 0;
}

inline Requirement::Requirement(Coord natural) {
    natural_ = natural;
    stretch_ = 0;
    shrink_ = 0;
    alignment_ = 0;
}

inline Requirement::Requirement(
    Coord natural, Coord stretch, Coord shrink, float alignment
) {
    natural_ = natural;
    stretch_ = stretch;
    shrink_ = shrink;
    alignment_ = alignment;
}

inline boolean Requirement::defined() const {
    return natural_ != -fil;
}

inline void Requirement::natural(Coord c) { natural_ = c; }
inline Coord Requirement::natural() const { return natural_; }
inline void Requirement::stretch(Coord c) { stretch_ = c; }
inline Coord Requirement::stretch() const { return stretch_; }
inline void Requirement::shrink(Coord c) { shrink_ = c; }
inline Coord Requirement::shrink() const { return shrink_; }
inline void Requirement::alignment(float a) { alignment_ = a; }
inline float Requirement::alignment() const { return alignment_; }

inline int Requisition::penalty() const { return penalty_; }
inline void Requisition::penalty(int penalty) { penalty_ = penalty; }

inline Allotment::Allotment() {
    origin_ = 0;
    span_ = 0;
    alignment_ = 0;
}

inline Allotment::Allotment(Coord origin, Coord span, float alignment) {
    origin_ = origin;
    span_ = span;
    alignment_ = alignment;
}

inline void Allotment::origin(Coord o) { origin_ = o; }
inline void Allotment::offset(Coord o) { origin_ += o; }
inline Coord Allotment::origin() const { return origin_; }
inline void Allotment::span(Coord c) { span_ = c; }
inline Coord Allotment::span() const { return span_; }
inline void Allotment::alignment(float a) { alignment_ = a; }
inline float Allotment::alignment() const { return alignment_; }

inline Coord Allotment::begin() const {
    return origin_ - Coord(alignment_ * span_);
}

inline Coord Allotment::end() const {
    return origin_ - Coord(alignment_ * span_) + span_;
}

inline void Allocation::allot_x(const Allotment& a) { x_ = a; }
inline void Allocation::allot_y(const Allotment& a) { y_ = a; }

inline Allotment& Allocation::x_allotment() { return x_; }
inline Allotment& Allocation::y_allotment() { return y_; }
inline const Allotment& Allocation::x_allotment() const { return x_; }
inline const Allotment& Allocation::y_allotment() const { return y_; }

inline Coord Allocation::x() const { return x_.origin(); }
inline Coord Allocation::y() const { return y_.origin(); }
inline Coord Allocation::left() const { return x_.begin(); }
inline Coord Allocation::right() const { return x_.end(); }
inline Coord Allocation::bottom() const { return y_.begin(); }
inline Coord Allocation::top() const { return y_.end(); }

inline void Extension::xy_extents(Coord x0, Coord x1, Coord y0, Coord y1) {
    x_begin_ = x0;
    x_end_ = x1;
    y_begin_ = y0;
    y_end_ = y1;
}

inline void Extension::xy_extents(const Allocation& a) {
    x_begin_ = a.left();
    x_end_ = a.right();
    y_begin_ = a.bottom();
    y_end_ = a.top();
}

inline void Extension::extend(const Extension& ext) {
    x_begin_ = Math::min(x_begin_, ext.x_begin_);
    x_end_ = Math::max(x_end_, ext.x_end_);
    y_begin_ = Math::min(y_begin_, ext.y_begin_);
    y_end_ = Math::max(y_end_, ext.y_end_);
}

inline void Extension::extend(const Allocation& a) {
    x_begin_ = Math::min(x_begin_, a.left());
    x_end_ = Math::max(x_end_, a.right());
    y_begin_ = Math::min(y_begin_, a.bottom());
    y_end_ = Math::max(y_end_, a.top());
}

inline Coord Extension::left() const { return x_begin_; }
inline Coord Extension::bottom() const { return y_begin_; }
inline Coord Extension::right() const { return x_end_; }
inline Coord Extension::top() const { return y_end_; }

#include <InterViews/_leave.h>

#endif
