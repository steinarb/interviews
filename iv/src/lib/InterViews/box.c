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

/*
 * Box - tiling glyph
 */

#include <InterViews/align.h>
#include <InterViews/box.h>
#include <InterViews/canvas.h>
#include <InterViews/hit.h>
#include <InterViews/printer.h>
#include <InterViews/superpose.h>
#include <InterViews/tile.h>
#include <OS/list.h>
#include <OS/math.h>

class BoxComponent {
public:
    Glyph* glyph_;
};

declareList(BoxComponent_List,BoxComponent);
implementList(BoxComponent_List,BoxComponent);

class BoxAllocation {
public:
    BoxAllocation();

    Allocation allocation_;
};

BoxAllocation::BoxAllocation() { }

declareList(BoxAllocation_List,BoxAllocation);
implementList(BoxAllocation_List,BoxAllocation);

static const float epsilon = 0.1;

Box::Box(Layout* layout, GlyphIndex size) : Glyph() {
    layout_ = layout;
    component_info_ = new BoxComponent_List(size);
    allocation_info_ = new BoxAllocation_List(size);
    requested_ = false;
    allocated_ = false;
    canvas_ = nil;
}

Box::Box(
    Layout* layout,
    Glyph* g1, Glyph* g2, Glyph* g3, Glyph* g4, Glyph* g5,
    Glyph* g6, Glyph* g7, Glyph* g8, Glyph* g9, Glyph* g10
) : Glyph() {
    layout_ = layout;
    component_info_ = new BoxComponent_List(4);
    allocation_info_ = new BoxAllocation_List(4);
    if (g1 != nil) {
        append(g1);
    }
    if (g2 != nil) {
        append(g2);
    }
    if (g3 != nil) {
        append(g3);
    }
    if (g4 != nil) {
        append(g4);
    }
    if (g5 != nil) {
        append(g5);
    }
    if (g6 != nil) {
        append(g6);
    }
    if (g7 != nil) {
        append(g7);
    }
    if (g8 != nil) {
        append(g8);
    }
    if (g9 != nil) {
        append(g9);
    }
    if (g10 != nil) {
        append(g10);
    }
    requested_ = false;
    allocated_ = false;
    canvas_ = nil;
}

Box::~Box() {
    GlyphIndex count = component_info_->count();
    for (GlyphIndex i = 0; i < count; ++i) {
        BoxComponent& info = component_info_->item(i);
	Resource::unref(info.glyph_);
    }
    delete component_info_;
    component_info_ = nil;
    delete allocation_info_;
    allocation_info_ = nil;
    delete layout_;
    layout_ = nil;
}

GlyphIndex Box::count() const {
    return component_info_->count();
}

Glyph* Box::component(GlyphIndex index) const {
    return component_info_->item(index).glyph_;
}

void Box::allotment(GlyphIndex index, DimensionName res, Allotment& a) const {
    a = allocation_info_->item(index).allocation_.allotment(res);
}

void Box::change(GlyphIndex) {
    requested_ = false;
    allocated_ = false;
    canvas_ = nil;
}

void Box::append(Glyph* glyph) {
    BoxComponent component;
    component.glyph_ = glyph;
    BoxAllocation allocation;
    component_info_->append(component);
    allocation_info_->append(allocation);
    Resource::ref(glyph);
    requested_ = false;
    allocated_ = false;
    canvas_ = nil;
}

void Box::prepend(Glyph* glyph) {
    BoxComponent component;
    component.glyph_ = glyph;
    BoxAllocation allocation;
    component_info_->prepend(component);
    allocation_info_->prepend(allocation);
    Resource::ref(glyph);
    requested_ = false;
    allocated_ = false;
    canvas_ = nil;
}

void Box::insert(GlyphIndex index, Glyph* glyph) {
    BoxComponent component;
    component.glyph_ = glyph;
    BoxAllocation allocation;
    component_info_->insert(index, component);
    allocation_info_->insert(index, allocation);
    Resource::ref(glyph);
    requested_ = false;
    allocated_ = false;
    canvas_ = nil;
}

void Box::remove(GlyphIndex index) {
    BoxComponent& component = component_info_->item(index);
    Resource::unref(component.glyph_);
    component_info_->remove(index);
    allocation_info_->remove(index);
    requested_ = false;
    allocated_ = false;
    canvas_ = nil;
}

void Box::replace(GlyphIndex index, Glyph* glyph) {
    BoxComponent& component = component_info_->item(index);
    Resource::ref(glyph);
    Resource::unref(component.glyph_);
    component.glyph_ = glyph;
    requested_ = false;
    allocated_ = false;
    canvas_ = nil;
}

void Box::request(Requisition& requisition) const {
    if (!requested_) {
        GlyphIndex count = component_info_->count();
        Requisition* r = new Requisition[count];
        for (GlyphIndex index = 0; index < count; ++index) {
            BoxComponent& component = component_info_->item(index);
            Glyph* glyph = component.glyph_;
            if (glyph != nil) {
                glyph->request(r[index]);
            }
        }
	Box* b = (Box*)this;
        b->layout_->request(count, r, b->requisition_);
        b->requested_ = true;
	delete r;
    }
    requisition = requisition_;
}

static boolean same_size(
    const Allotment& a1, const Allotment& a2, float epsilon
) {
    if (!Math::equal(a1.span(), a2.span(), epsilon)) {
        return false;
    } else if (!Math::equal(a1.alignment(), a2.alignment(), epsilon)) {
        return false;
    } else {
        return true;
    }
}

static void offset_allocate(
    Canvas* canvas, const Allocation&,
    Extension& extension,
    Coord dx, Coord dy,
    BoxComponent_List* component_info,
    BoxAllocation_List* allocation_info
) {
    Extension child_ext;
    Extension empty_ext;
    empty_ext.xy_extents(fil, -fil, fil, -fil);
    GlyphIndex count = component_info->count();
    GlyphIndex index;
    for (index = 0; index < count; ++index) {
        Glyph* glyph = component_info->item(index).glyph_;
        Allocation& a = allocation_info->item(index).allocation_;
        Allotment ax = a.x_allotment();
        Allotment ay = a.y_allotment();
        ax.offset(dx);
        ay.offset(dy);
        a.allot_x(ax);
        a.allot_y(ay);
        if (glyph != nil) {
	    child_ext = empty_ext;
            glyph->allocate(canvas, a, child_ext);
	    extension.extend(child_ext);
        }
    }
}

static void redo_allocate(
    Canvas* canvas, const Allocation& allocation,
    Extension& extension,
    Layout* layout,
    BoxComponent_List* component_info,
    BoxAllocation_List* allocation_info
) {
    GlyphIndex count = component_info->count();
    Allocation* allocations = new Allocation[count];
    Requisition* requisitions = new Requisition[count];
    GlyphIndex index;
    for (index = 0; index < count; ++index) {
        Glyph* glyph = component_info->item(index).glyph_;
        if (glyph != nil) {
            glyph->request(requisitions[index]);
        }
    }
    layout->allocate(allocation, count, requisitions, allocations);
    for (index = 0; index < count; ++index) {
        Glyph* glyph = component_info->item(index).glyph_;
        Allocation& a = allocation_info->item(index).allocation_;
        a = allocations[index];
        Extension b;
	b.xy_extents(fil, -fil, fil, -fil);
        if (glyph != nil) {
            glyph->allocate(canvas, a, b);
        }
        extension.extend(b);
    }
    delete allocations;
    delete requisitions;
}

void Box::allocate(Canvas* c, const Allocation& a, Extension& ext) {
    const Allotment& x = a.x_allotment();
    const Allotment& y = a.y_allotment();
    Allotment& oldx = allocation_.x_allotment();
    Allotment& oldy = allocation_.y_allotment();
    if (
        allocated_ && canvas_ == c &&
        same_size(x, oldx, epsilon) && same_size(y, oldy, epsilon)
    ) {
        Coord dx = x.origin() - oldx.origin();
        Coord dy = y.origin() - oldy.origin();
        if (dx < epsilon && dx > -epsilon && dy < epsilon && dy > -epsilon) {
            ext = extension_;
        } else {
            extension_ = ext;
            offset_allocate(
                c, a, extension_,
                dx, dy, component_info_, allocation_info_
            );
	    Allotment ax(oldx.origin() + dx, oldx.span(), oldx.alignment());
	    Allotment ay(oldy.origin() + dy, oldy.span(), oldy.alignment());
            allocation_.allot_x(ax);
            allocation_.allot_y(ay);
            ext = extension_;
        }
    } else {
        extension_ = ext;
        redo_allocate(
            c, a, extension_,
            layout_, component_info_, allocation_info_
        );
        allocation_ = a;
        allocated_ = true;
	canvas_ = c;
        ext = extension_;
    }
}

void Box::draw(Canvas* canvas, const Allocation&) const {
    boolean damaged = canvas->damaged(
        extension_.left(), extension_.bottom(),
        extension_.right(), extension_.top()
    );
    if (damaged) {
        GlyphIndex count = component_info_->count();
        for (GlyphIndex index = 0; index < count; ++index) {
            Glyph* glyph = component_info_->item(index).glyph_;
            Allocation& a = allocation_info_->item(index).allocation_;
            if (glyph != nil) {
                glyph->draw(canvas, a);
            }
        }
    }
}

void Box::print(Printer* p, const Allocation&) const {
    boolean damaged = p->damaged(
        extension_.left(), extension_.bottom(),
        extension_.right(), extension_.top()
    );
    if (damaged) {
        GlyphIndex count = component_info_->count();
        for (GlyphIndex index = 0; index < count; ++index) {
            Glyph* glyph = component_info_->item(index).glyph_;
            Allocation& a = allocation_info_->item(index).allocation_;
            if (glyph != nil) {
                glyph->print(p, a);
            }
        }
    }
}

void Box::pick(Canvas* c, const Allocation&, int depth, Hit& h) {
    Coord x = h.left();
    Coord y = h.bottom();
    GlyphIndex count = component_info_->count();
    for (GlyphIndex index = 0; index < count; ++index) {
        Glyph* glyph = component_info_->item(index).glyph_;
        if (glyph != nil) {
            Allocation& a = allocation_info_->item(index).allocation_;
            if (
                x >= a.left() && x < a.right()
                && y >= a.bottom() && y < a.top()
            ) {
		h.begin(depth, this, index);
		glyph->pick(c, a, depth + 1, h);
		h.end();
            }
        }
    }
}

LRBox::LRBox(
    GlyphIndex size
) : Box(
    new Superpose(new Tile(Dimension_X), new Align(Dimension_Y)),
    size
) { }

LRBox::LRBox(
    Glyph* g1, Glyph* g2, Glyph* g3, Glyph* g4, Glyph* g5,
    Glyph* g6, Glyph* g7, Glyph* g8, Glyph* g9, Glyph* g10
) : Box(
    new Superpose(new Tile(Dimension_X), new Align(Dimension_Y)),
    g1, g2, g3, g4, g5, g6, g7, g8, g9, g10
) { }

LRBox::~LRBox() { }

TBBox::TBBox(
    GlyphIndex size
) : Box(
    new Superpose(new TileReversed(Dimension_Y), new Align(Dimension_X)),
    size
) { }

TBBox::TBBox(
    Glyph* g1, Glyph* g2, Glyph* g3, Glyph* g4, Glyph* g5,
    Glyph* g6, Glyph* g7, Glyph* g8, Glyph* g9, Glyph* g10
) : Box(
    new Superpose(new TileReversed(Dimension_Y), new Align(Dimension_X)),
    g1, g2, g3, g4, g5, g6, g7, g8, g9, g10
) { }

TBBox::~TBBox() { }

Overlay::Overlay(
    Glyph* g1, Glyph* g2, Glyph* g3, Glyph* g4, Glyph* g5,
    Glyph* g6, Glyph* g7, Glyph* g8, Glyph* g9, Glyph* g10
) : Box(
    new Superpose(new Align(Dimension_X), new Align(Dimension_Y)),
    g1, g2, g3, g4, g5, g6, g7, g8, g9, g10
) { }

Overlay::~Overlay() { }
