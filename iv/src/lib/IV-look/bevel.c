/*
 * Copyright (c) 1991 Stanford University
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
 * Bevel - 3D framing of a glyph
 */

#include <IV-look/bevel.h>
#include <InterViews/canvas.h>
#include <InterViews/color.h>
#include <InterViews/margin.h>
#include <math.h>

Bevel::Bevel(
    Glyph* g, Beveler b,
    const Color* light, const Color* medium, const Color* dark,
    Coord thickness
) : MonoGlyph(new Margin(g, thickness)) {
    beveler_ = b;
    light_ = light;
    medium_ = medium;
    dark_ = dark;
    thickness_ = thickness;
    Resource::ref(light_);
    Resource::ref(medium_);
    Resource::ref(dark_);
}

Bevel::~Bevel() {
    Resource::unref(light_);
    Resource::unref(medium_);
    Resource::unref(dark_);
}

void Bevel::allocate(Canvas* canvas, const Allocation& a, Extension& ext) {
    MonoGlyph::allocate(canvas, a, ext);
    ext.extend(a);
}

void Bevel::draw(Canvas* c, const Allocation& a) const {
    Coord left = a.left();
    Coord bottom = a.bottom();
    Coord right = a.right();
    Coord top = a.top();
    if (c->damaged(left, bottom, right, top)) {
	(*beveler_)(
	    c, light_, medium_, dark_, thickness_, left, bottom, right, top
	);
    }
    MonoGlyph::draw(c, a);
}

void Bevel::rect(
    Canvas* c, const Color* light, const Color* medium, const Color* dark,
    Coord thickness, Coord left, Coord bottom, Coord right, Coord top
) {
    /* background */
    c->fill_rect(left, bottom, right, top, medium);

    Coord left_inside = left + thickness;
    Coord bottom_inside = bottom + thickness;
    Coord right_inside = right - thickness;
    Coord top_inside = top - thickness;

    /* left edge */
    c->new_path();
    c->move_to(left, bottom);
    c->line_to(left, top);
    c->line_to(left_inside, top);
    c->line_to(left_inside, bottom);
    c->close_path();
    c->fill(light);

    /* top edge */
    c->new_path();
    c->move_to(left_inside, top_inside);
    c->line_to(left_inside, top);
    c->line_to(right, top);
    c->line_to(right, top_inside);
    c->close_path();
    c->fill(light);

    /* right edge */
    c->new_path();
    c->move_to(right_inside, bottom_inside);
    c->line_to(right_inside, top_inside);
    c->line_to(right, top);
    c->line_to(right, bottom);

    /* bottom edge (as part of same path) */
    c->line_to(left, bottom);
    c->line_to(left_inside, bottom_inside);
    c->line_to(right_inside, bottom_inside);
    c->close_path();
    c->fill(dark);
}

/*
 * A bit of a misnomer to call these arrows; they are really beveled
 * triangles.  The only tricky part is dropping the bevel down and to the
 * right of the original triangle, which requires finding delta x,y for
 * a given thickness and matching the slope of the triangle.
 */

void Bevel::left_arrow(
    Canvas* c, const Color* light, const Color* medium, const Color* dark,
    Coord thickness, Coord left, Coord bottom, Coord right, Coord top
) {
    Coord center_y = (bottom + top) / 2;
    float slope = ((top - bottom) / (right - left)) / 2;
    float delta_x = thickness / sqrt(slope * slope + 1);
    float delta_y = slope * delta_x;

    c->new_path();
    c->move_to(left, center_y);
    c->line_to(right, top);
    c->line_to(right, bottom);
    c->close_path();
    c->fill(medium);

    c->new_path();
    c->move_to(right - thickness, bottom + thickness);
    c->line_to(right - thickness, top - thickness);
    c->line_to(right, top);
    c->line_to(right, bottom);
    c->close_path();
    c->fill(dark);

    c->new_path();
    c->move_to(left, center_y);
    c->line_to(left + thickness, center_y);
    c->line_to(right - thickness, bottom + thickness);
    c->line_to(right, bottom);
    c->close_path();
    c->fill(dark);

    c->new_path();
    c->move_to(left + delta_x, center_y - delta_y);
    c->line_to(left, center_y);
    c->line_to(right, top);
    c->line_to(right, top - thickness);
    c->close_path();
    c->fill(light);
}

void Bevel::right_arrow(
    Canvas* c, const Color* light, const Color* medium, const Color* dark,
    Coord thickness, Coord left, Coord bottom, Coord right, Coord top
) {
    Coord center_y = (bottom + top) / 2;
    float slope = ((top - bottom) / (right - left)) / 2;
    float delta_x = thickness / sqrt(slope * slope + 1);
    float delta_y = slope * delta_x;

    c->new_path();
    c->move_to(left, bottom);
    c->line_to(left, top);
    c->line_to(right, center_y);
    c->close_path();
    c->fill(medium);

    c->new_path();
    c->move_to(left, bottom);
    c->line_to(left + thickness, bottom + thickness);
    c->line_to(right - thickness, center_y);
    c->line_to(right, center_y);
    c->close_path();
    c->fill(dark);

    c->new_path();
    c->move_to(left, bottom);
    c->line_to(left, top);
    c->line_to(left + thickness, top - thickness);
    c->line_to(left + delta_x, bottom + delta_y);
    c->close_path();
    c->fill(light);

    c->new_path();
    c->move_to(left + thickness, top - thickness);
    c->line_to(left, top);
    c->line_to(right, center_y);
    c->line_to(right - delta_x, center_y - delta_y);
    c->close_path();
    c->fill(light);
}

void Bevel::up_arrow(
    Canvas* c, const Color* light, const Color* medium, const Color* dark,
    Coord thickness, Coord left, Coord bottom, Coord right, Coord top
) {
    Coord center_x = (left + right) / 2;
    float slope = 2 * ((top - bottom) / (right - left));
    float delta_x = thickness / sqrt(slope * slope + 1);
    float delta_y = slope * delta_x;

    c->new_path();
    c->move_to(left, bottom);
    c->line_to(center_x, top);
    c->line_to(right, bottom);
    c->close_path();
    c->fill(medium);

    c->new_path();
    c->move_to(left, bottom);
    c->line_to(right, bottom);
    c->line_to(right - thickness, bottom + thickness);
    c->line_to(left + thickness, bottom + thickness);
    c->close_path();
    c->fill(dark);

    c->new_path();
    c->move_to(center_x, top - thickness);
    c->line_to(center_x, top);
    c->line_to(right, bottom);
    c->line_to(right - thickness, bottom + thickness);
    c->close_path();
    c->fill(dark);

    c->new_path();
    c->move_to(left, bottom);
    c->line_to(left + delta_x, bottom);
    c->line_to(center_x + delta_x, top - delta_y);
    c->line_to(center_x, top);
    c->close_path();
    c->fill(light);
}

void Bevel::down_arrow(
    Canvas* c, const Color* light, const Color* medium, const Color* dark,
    Coord thickness, Coord left, Coord bottom, Coord right, Coord top
) {
    Coord center_x = (left + right) / 2;
    float slope = 2* ((top - bottom) / (right - left));
    float delta_x = thickness / sqrt(slope * slope + 1);
    float delta_y = slope * delta_x;

    c->new_path();
    c->move_to(left, top);
    c->line_to(right, top);
    c->line_to(center_x, bottom);
    c->close_path();
    c->fill(medium);

    c->new_path();
    c->move_to(center_x, bottom);
    c->line_to(center_x, bottom + thickness);
    c->line_to(right - thickness, top - thickness);
    c->line_to(right, top);
    c->close_path();
    c->fill(dark);

    c->new_path();
    c->move_to(left, top);
    c->line_to(center_x, bottom);
    c->line_to(center_x + delta_x, bottom + delta_y);
    c->line_to(left + thickness, top - thickness);
    c->close_path();
    c->fill(light);

    c->new_path();
    c->move_to(left, top);
    c->line_to(right, top);
    c->line_to(right - delta_x, top - delta_y);
    c->line_to(left + thickness, top - thickness);
    c->close_path();
    c->fill(light);
}
