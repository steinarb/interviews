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

#include <InterViews/compositor.h>
#include <InterViews/geometry.h>
#include <InterViews/glyph.h>
#include <OS/math.h>

static long best_new_sizes[] = {
    48, 112, 240, 496, 1008, 2032, 4080, 8176,
    16368, 32752, 65520, 131056, 262128, 524272, 1048560,
    2097136, 4194288, 8388592, 16777200, 33554416, 67108848
};

long best_new_count(long count, unsigned int size) {
    for (int i = 0; i < sizeof(best_new_sizes)/sizeof(int); ++i) {
        if (count * size < best_new_sizes[i]) {
            return best_new_sizes[i] / size;
        }
    }
    return count;
}

Requirement::Requirement(
    Coord natural_lead, Coord max_lead, Coord min_lead,
    Coord natural_trail, Coord max_trail, Coord min_trail
) {
    natural_lead = Math::max(min_lead, Math::min(max_lead, natural_lead));
    max_lead = Math::max(max_lead, natural_lead);
    min_lead = Math::min(min_lead, natural_lead);
    natural_trail = Math::max(min_trail, Math::min(max_trail, natural_trail));
    max_trail = Math::max(max_trail, natural_trail);
    min_trail = Math::min(min_trail, natural_trail);
    natural_ = natural_lead + natural_trail;
    if (natural_lead == 0) {
        shrink_ = natural_trail - min_trail;
        stretch_ = max_trail - natural_trail;
        alignment_ = 0;
    } else if (natural_trail == 0) {
        shrink_ = natural_lead - min_lead;
        stretch_ = max_lead - natural_lead;
        alignment_ = 1;
    } else {
        float fshrink = Math::max(
            float(min_lead)/float(natural_lead),
            float(min_trail)/float(natural_trail)
        );
        shrink_ = Coord(natural_ * (1 - fshrink));
        float fstretch = Math::min(
            float(max_lead)/float(natural_lead),
            float(max_trail)/float(natural_trail)
        );
        stretch_ = Coord(natural_ * (fstretch - 1));
        if (natural_ == 0) {
            alignment_ = 0;
        } else {
            alignment_ = float(natural_lead) / float(natural_);
        }
    }
}

boolean Requirement::equals(const Requirement& r, float epsilon) const {
    if (!Math::equal(natural_, r.natural_, epsilon)) {
        return false;
    } else if (!Math::equal(stretch_, r.stretch_, epsilon)) {
        return false;
    } else if (!Math::equal(shrink_, r.shrink_, epsilon)) {
        return false;
    } else if (!Math::equal(alignment_, r.alignment_, epsilon)) {
        return false;
    } else {
        return true;
    }
}

/* hack to persuade cfront to inline both subobject constructors */

Requisition::Requisition() : x_(), y_(-fil) {
    penalty_ = PenaltyBad;
}

Requisition::Requisition(const Requisition& r) : x_(r.x_), y_(r.y_) {
    penalty_ = r.penalty_;
}

boolean Requisition::equals(const Requisition& r, float epsilon) const {
    return x_.equals(r.x_, epsilon) && y_.equals(r.y_, epsilon);
}

void Requisition::require(DimensionName n, const Requirement& r) {
    switch (n) {
    case Dimension_X:
        x_ = r;
        break;
    case Dimension_Y:
        y_ = r;
        break;
    }
}

Requirement& Requisition::requirement(DimensionName n) {
    switch (n) {
    case Dimension_X:
        return x_;
    case Dimension_Y:
        return y_;
    }
}

const Requirement& Requisition::requirement(DimensionName n) const {
    switch (n) {
    case Dimension_X:
        return x_;
    case Dimension_Y:
        return y_;
    }
}

boolean Allotment::equals(const Allotment& a, float epsilon) const {
    if (!Math::equal(origin_, a.origin_, epsilon)) {
        return false;
    } else if (!Math::equal(span_, a.span_, epsilon)) {
        return false;
    } else if (!Math::equal(alignment_, a.alignment_, epsilon)) {
        return false;
    } else {
        return true;
    }
}

/* hack to persuade cfront to inline both subobject constructors */

Allocation::Allocation() : x_(), y_(0, 0, 0) { }

Allocation::Allocation(const Allocation& a) : x_(a.x_), y_(a.y_) { }

void Allocation::allot(DimensionName n, const Allotment& a) {
    if (n == Dimension_X) {
	x_ = a;
    } else if (n == Dimension_Y) {
	y_ = a;
    }
}

static Allotment* empty_allotment;

Allotment& Allocation::allotment(DimensionName n) {
    if (n == Dimension_X) {
	return x_;
    } else if (n == Dimension_Y) {
	return y_;
    }
    if (empty_allotment == nil) {
	empty_allotment = new Allotment;
    }
    return *empty_allotment;
}

const Allotment& Allocation::allotment(DimensionName n) const {
    if (n == Dimension_X) {
	return x_;
    } else if (n == Dimension_Y) {
	return y_;
    }
    if (empty_allotment == nil) {
	empty_allotment = new Allotment;
    }
    return *empty_allotment;
}

boolean Allocation::equals(const Allocation& a, float epsilon) const {
    return x_.equals(a.x_, epsilon) && y_.equals(a.y_, epsilon);
}

Extension::Extension() {
    const Coord zero = 0;
    x_begin_ = zero;
    x_end_ = zero;
    y_begin_ = zero;
    y_end_ = zero;
}

Extension::Extension(const Extension& ext) {
    x_begin_ = ext.x_begin_;
    x_end_ = ext.x_end_;
    y_begin_ = ext.y_begin_;
    y_end_ = ext.y_end_;
}

void Extension::extent(DimensionName n, Coord begin, Coord end) {
    switch (n) {
    case Dimension_X:
	x_begin_ = begin;
	x_end_ = end;
        break;
    case Dimension_Y:
	y_begin_ = begin;
	y_end_ = end;
        break;
    }
}

void Extension::get_extent(DimensionName n, Coord& begin, Coord& end) const {
    switch (n) {
    case Dimension_X:
	begin = x_begin_;
	end = x_end_;
	break;
    case Dimension_Y:
	begin = y_begin_;
	end = y_end_;
        break;
    default:
	begin = 0;
	end = 0;
	break;
    }
}
