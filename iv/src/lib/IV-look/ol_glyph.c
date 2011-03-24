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
 * Common OpenLook objects
 */

#include <IV-look/ol_glyph.h>
#include <IV-look/ol_style.h>
#include <InterViews/brush.h>
#include <InterViews/canvas.h>
#include <InterViews/display.h>
#include <InterViews/font.h>
#include <InterViews/margin.h>
#include <InterViews/session.h>
#include <InterViews/window.h>
#include <OS/math.h>

/*
 * character definitions
 */
#define OLG_VSB_ELEVATOR                        1
#define OLG_VSB_ELEVATOR_LINE_BACKWARD          2
#define OLG_VSB_ELEVATOR_ABSOLUTE               3
#define OLG_VSB_ELEVATOR_LINE_FORWARD           4
#define OLG_VSB_REDUCED_ELEVATOR                5
#define OLG_VSB_REDUCED_ELEVATOR_LINE_BACKWARD  6
#define OLG_VSB_REDUCED_ELEVATOR_LINE_FORWARD   7
#define OLG_VSB_ANCHOR                          8
#define OLG_VSB_ANCHOR_INVERTED                 9
#define OLG_HSB_ELEVATOR                        10
#define OLG_HSB_ELEVATOR_LINE_BACKWARD          11
#define OLG_HSB_ELEVATOR_ABSOLUTE               12
#define OLG_HSB_ELEVATOR_LINE_FORWARD           13
#define OLG_HSB_REDUCED_ELEVATOR                14
#define OLG_HSB_REDUCED_ELEVATOR_LINE_BACKWARD  15
#define OLG_HSB_REDUCED_ELEVATOR_LINE_FORWARD   16
#define OLG_HSB_ANCHOR                          17
#define OLG_HSB_ANCHOR_INVERTED                 18
#define OLG_MENU_PIN_OUT                        19
#define OLG_MENU_PIN_IN                         20
#define OLG_MENU_DEFAULT_PIN_OUT                21
#define OLG_ABBREV_MENU_BUTTON                  22
#define OLG_ABBREV_MENU_BUTTON_INVERTED         23
/* new extension */
#define BUTTON_UL				24
#define BUTTON_LL				25
#define BUTTON_LEFT_ENDCAP_FILL			26
#define BUTTON_LR				27
#define BUTTON_UR				28
#define BUTTON_RIGHT_ENDCAP_FILL		29
#define BUTTON_TOP_1				30
#define BUTTON_TOP_2				31
#define BUTTON_TOP_4				32
#define BUTTON_TOP_8				33
#define BUTTON_TOP_16				34
#define BUTTON_BOTTOM_1				35
#define BUTTON_BOTTOM_2				36
#define BUTTON_BOTTOM_4				37
#define BUTTON_BOTTOM_8				38
#define BUTTON_BOTTOM_16			39
#define BUTTON_FILL_1				40
#define BUTTON_FILL_2				41
#define BUTTON_FILL_4				42
#define BUTTON_FILL_8				43
#define BUTTON_FILL_16				44
#define VERT_MENU_MARK_UL			45
#define VERT_MENU_MARK_LR			46
#define VERT_MENU_MARK_FILL			47
#define HORIZ_MENU_MARK_UL			48
#define HORIZ_MENU_MARK_LR			49
#define HORIZ_MENU_MARK_FILL			50
#define ABBREV_MENU_UL				51
#define ABBREV_MENU_LR				52
#define ABBREV_MENU_FILL			53
#define VERT_SB_UL				54
#define VERT_SB_LR				55
#define VERT_SB_TOPBOX_FILL			56
#define HORIZ_SB_UL				57
#define HORIZ_SB_LR				58
#define VERT_SB_BOTBOX_FILL			59
#define HORIZ_SLIDER_CONTROL_UL			60
#define HORIZ_SLIDER_CONTROL_LR			61
#define HORIZ_SLIDER_CONTROL_FILL		62
#define HORIZ_SLIDER_UL				63
#define HORIZ_SLIDER_LL				64
#define HORIZ_SLIDER_UR				65
#define HORIZ_SLIDER_LR				66
#define HORIZ_SLIDER_BOTTOM_1			67
#define HORIZ_SLIDER_BOTTOM_2			68
#define HORIZ_SLIDER_BOTTOM_4			69
#define HORIZ_SLIDER_BOTTOM_8			70
#define HORIZ_SLIDER_BOTTOM_16			71
#define HORIZ_SLIDER_FILL_1			72
#define HORIZ_SLIDER_FILL_2			73
#define HORIZ_SLIDER_FILL_4			74
#define HORIZ_SLIDER_FILL_8			75
#define HORIZ_SLIDER_FILL_16			76
#define HORIZ_SLIDER_LEFT_ENDCAP_FILL		77
#define HORIZ_SLIDER_RIGHT_ENDCAP_FILL		78
#define VERT_SLIDER_UL				79
#define VERT_SLIDER_UR				80
#define VERT_SLIDER_TOP_ENDCAP_FILL		81
#define VERT_SLIDER_LL				82
#define VERT_SLIDER_LR				83
#define VERT_SLIDER_BOTTOM_ENDCAP_FILL		84
#define VERT_SLIDER_CONTROL_UL			85
#define VERT_SLIDER_CONTROL_LR			86
#define VERT_SLIDER_CONTROL_FILL		87
#define UL_RESIZE_UL				88
#define UL_RESIZE_LR				89
#define UL_RESIZE_FILL				90
#define UR_RESIZE_UL				91
#define UR_RESIZE_LR				92
#define UR_RESIZE_FILL				93
#define LR_RESIZE_UL				94
#define LR_RESIZE_LR				95
#define LR_RESIZE_FILL				96
#define LL_RESIZE_UL				97
#define LL_RESIZE_LR				98
#define LL_RESIZE_FILL				99
#define PUSHPIN_OUT_TOP				100
#define PUSHPIN_OUT_BOTTOM			101
#define PUSHPIN_OUT_MIDDLE			102
#define PUSHPIN_IN_TOP				103
#define PUSHPIN_IN_BOTTOM			104
#define PUSHPIN_IN_MIDDLE			105
#define DFLT_BUTTON_LEFT_ENDCAP			106
#define DFLT_BUTTON_RIGHT_ENDCAP		107
#define DFLT_BUTTON_MIDDLE_1			108
#define DFLT_BUTTON_MIDDLE_2			109
#define DFLT_BUTTON_MIDDLE_4			110
#define DFLT_BUTTON_MIDDLE_8			111
#define DFLT_BUTTON_MIDDLE_16			112
#define BASE_OFF_SPECIALCHAR			113 /*special char */
#define UNCHECKED_BOX_UL			114
#define UNCHECKED_BOX_LR			115
#define UNCHECKED_BOX_FILL			116
#define CHECK_MARK				117
#define CHECKED_BOX_FILL			118
#define UNCHECKED_BOX_OUTLINE			119
#define HORIZ_GAUGE_UL				120
#define HORIZ_GAUGE_LL				121
#define HORIZ_GAUGE_UR				122
#define HORIZ_GAUGE_LR				123
#define HORIZ_GAUGE_BOTTOM_1			124
#define HORIZ_GAUGE_BOTTOM_2			125
#define HORIZ_GAUGE_BOTTOM_4			126
#define HORIZ_GAUGE_BOTTOM_8			127
#define HORIZ_GAUGE_BOTTOM_16			128
#define VERT_GAUGE_UL				129
#define VERT_GAUGE_UR				130
#define VERT_GAUGE_LL				131
#define VERT_GAUGE_LR				132
#define VERT_ABBREV_SB_UL			133
#define VERT_ABBREV_SB_LR			134
#define HORIZ_SB_RIGHTBOX_FILL			135
#define HORIZ_ABBREV_SB_UL			136
#define HORIZ_ABBREV_SB_LR			137
#define HORIZ_SB_LEFTBOX_FILL			138
#define BUTTON_OUTLINE_LEFT_ENDCAP		139
#define BUTTON_OUTLINE_RIGHT_ENDCAP		140
#define BUTTON_OUTLINE_MIDDLE_1			141
#define BUTTON_OUTLINE_MIDDLE_2			142
#define BUTTON_OUTLINE_MIDDLE_4			143
#define BUTTON_OUTLINE_MIDDLE_8			144
#define BUTTON_OUTLINE_MIDDLE_16		145
#define BUTTON_FILL_2D_LEFTENDCAP		146
#define BUTTON_FILL_2D_RIGHTENDCAP      	147
#define BUTTON_FILL_2D_MIDDLE_1     		148
#define BUTTON_FILL_2D_MIDDLE_2 		149
#define BUTTON_FILL_2D_MIDDLE_4 		150
#define BUTTON_FILL_2D_MIDDLE_8 		151
#define BUTTON_FILL_2D_MIDDLE_16		152
#define MENU_DFLT_OUTLINE_LEFT_ENDCAP           153
#define MENU_DFLT_OUTLINE_RIGHT_ENDCAP          154
#define MENU_DFLT_OUTLINE_MIDDLE_1              155
#define MENU_DFLT_OUTLINE_MIDDLE_2              156
#define MENU_DFLT_OUTLINE_MIDDLE_4              157
#define MENU_DFLT_OUTLINE_MIDDLE_8              158
#define MENU_DFLT_OUTLINE_MIDDLE_16             159
#define PIXLABEL_BUTTON_UL			160 
#define PIXLABEL_BUTTON_LL			161
#define UL_RESIZE_OUTLINE			162
#define UR_RESIZE_OUTLINE			163
#define LR_RESIZE_OUTLINE			164
#define LL_RESIZE_OUTLINE			165
#define VERT_SB_NO_BACK_OUTLINE                 166
#define VERT_SB_NO_FWD_OUTLINE                  167
#define VERT_SB_INACTIVE_OUTLINE                168
#define HORIZ_SB_NO_BACK_OUTLINE                169
#define HORIZ_SB_NO_FWD_OUTLINE                 170
#define HORIZ_SB_INACTIVE_OUTLINE               171
#define HORIZ_SLIDER_CONTROL_OUTLINE		172
#define HORIZ_SLIDER_LEFT_ENDCAP_OUTLINE	173
#define	HORIZ_SLIDER_RIGHT_ENDCAP_OUTLINE	174
#define HORIZ_SLIDER_OUTLINE_1			175
#define HORIZ_SLIDER_OUTLINE_2			176
#define HORIZ_SLIDER_OUTLINE_4			177
#define HORIZ_SLIDER_OUTLINE_8			178
#define HORIZ_SLIDER_OUTLINE_16			179
#define VERT_SLIDER_TOP_ENDCAP_OUTLINE		180
#define VERT_SLIDER_BOTTOM_ENDCAP_OUTLINE	181
#define VERT_SLIDER_CONTROL_OUTLINE		182
#define PUSHPIN_OUT_DEFAULT_TOP 		183
#define PUSHPIN_OUT_DEFAULT_BOTTOM 		184
#define PUSHPIN_OUT_DEFAULT_MIDDLE 		185
#define HORIZ_GAUGE_LEFT_ENDCAP_OUTLINE		186
#define HORIZ_GAUGE_RIGHT_ENDCAP_OUTLINE	187
#define HORIZ_GAUGE_OUTLINE_MIDDLE_1		188
#define HORIZ_GAUGE_OUTLINE_MIDDLE_2		189
#define HORIZ_GAUGE_OUTLINE_MIDDLE_4		190
#define HORIZ_GAUGE_OUTLINE_MIDDLE_8		191
#define HORIZ_GAUGE_OUTLINE_MIDDLE_16		192
#define CHECK_BOX_CLEAR_FILL			193
#define VERT_SB_BOX_UL 				194
#define VERT_SB_BOX_LR 				195
#define DIMPLE_UL				196
#define DIMPLE_LR				197
#define DIMPLE_FILL				198
#define SLIDER_CHANNEL_OFFSET			199 /* special char */
#define HORIZ_SB_BOX_UL				200
#define HORIZ_SB_BOX_LR				201
#define VERT_BACK_MENU_MARK_UL			202
#define VERT_BACK_MENU_MARK_LR			203
#define VERT_BACK_MENU_MARK_FILL		204
#define HORIZ_BACK_MENU_MARK_UL			205
#define HORIZ_BACK_MENU_MARK_LR			206
#define HORIZ_BACK_MENU_MARK_FILL		207
#define	OLGX_ACTIVE_CARET			208
#define OLGX_INACTIVE_CARET			209
#define VERT_GAUGE_TOPENDCAP   			210
#define VERT_GAUGE_BOTENDCAP   			211
#define PIXLABEL_BUTTON_UR   			212
#define PIXLABEL_BUTTON_LR   			213
#define PIXLABEL_BUTTON_2D_LR 			214
#define PIXLABEL_DEF_BUTTON_UL 			215
#define PIXLABEL_DEF_BUTTON_LL 			216
#define PIXLABEL_DEF_BUTTON_UR 			217
#define PIXLABEL_DEF_BUTTON_LR 			218
#define HORIZ_GAUGE_LEFT_ENDFILL                219
#define HORIZ_GAUGE_MIDDLE_FILL_1               220
#define HORIZ_GAUGE_MIDDLE_FILL_2               221
#define HORIZ_GAUGE_MIDDLE_FILL_4               222
#define HORIZ_GAUGE_MIDDLE_FILL_8               223
#define HORIZ_GAUGE_MIDDLE_FILL_16              224
#define HORIZ_GAUGE_RIGHT_ENDFILL               225
#define VERT_GAUGE_TOP_FILL                     226
#define VERT_GAUGE_BOT_FILL                     227
#define TEXTSCROLLBUTTON_LEFT                   228
#define TEXTSCROLLBUTTON_RIGHT                  229
#define TEXTSCROLLBUTTON_LEFT_INV               230
#define TEXTSCROLLBUTTON_RIGHT_INV              231
#define NUMERIC_SCROLL_BUTTON_NORMAL            232
#define NUMERIC_SCROLL_BUTTON_LEFT_INV          233
#define NUMERIC_SCROLL_BUTTON_RIGHT_INV         234

static const int l1 = 2;
static const int l2 = 3;
static const int l3 = 6;
static const int endcap = 6;
static const int normal_inset = 1;
static const int default_inset = 3;
static const int height = 2 * (endcap + default_inset) + 3;

/** class OL_ButtonGlyph */

OL_ButtonGlyph::OL_ButtonGlyph(
    Glyph* label, OL_Style* s,
    boolean invoked, boolean inactive, boolean menu, boolean def
) : MonoGlyph(nil) {
    style_ = s;
    Resource::ref(style_);
    invoked_ = invoked;
    inactive_ = inactive;
    menu_ = menu;
    default_ = def;
    Display* d = Session::instance()->default_display();
    Coord inset_coord = d->to_coord(default_inset);
    body(new Margin(label, inset_coord, inset_coord));
}

OL_ButtonGlyph::~OL_ButtonGlyph() {
    Resource::unref(style_);
}

void OL_ButtonGlyph::request(Requisition& requisition) const {
    MonoGlyph::request(requisition);
    Display* d = Session::instance()->default_display();
    Requirement ry = requisition.requirement(Dimension_Y);
    ry.natural(Math::max(ry.natural(), d->to_coord(height)));
    ry.shrink(Math::min(ry.shrink(), ry.natural() - d->to_coord(height)));
    requisition.require(Dimension_Y, ry);
}

void OL_ButtonGlyph::allocate(
    Canvas* canvas, const Allocation& a, Extension& ext
) {
    MonoGlyph::allocate(canvas, a, ext);
    ext.extend(a);
}

void OL_ButtonGlyph::draw(Canvas* canvas, const Allocation& a) const {
    Coord l = a.left();
    Coord r = a.right();
    Coord b = a.bottom();
    Coord t = a.top();

    const Color* top, * bottom, * fill, * gray;
    if (invoked_) {
	top = style_->dull();
	bottom = style_->bright();
	fill = style_->fill_in();
	gray = style_->gray_in();
    } else {
	top = style_->bright();
	bottom = style_->dull();
	fill = style_->fill_out();
	gray = style_->gray_out();
    }

    path(canvas, 1, l, b, r, t);
    canvas->fill(fill);

    MonoGlyph::draw(canvas, a);

    if (inactive_) {
        canvas->fill(gray);
    }

    Brush* brush = new Brush(0);
    Resource::ref(brush);
    if (invoked_ || !menu_) {
        top_path(canvas, 1, l, b, r, t);
        canvas->stroke(top, brush);
        bottom_path(canvas, 1, l, b, r, t);
        canvas->stroke(bottom, brush);
    }
    if (default_) {
        path(canvas, 3, l, b, r, t);
        canvas->stroke(style_->foreground(), brush);
    }
    Resource::unref(brush);
}

void OL_ButtonGlyph::path(
    Canvas* canvas, int inset, Coord l, Coord b, Coord r, Coord t
) const {
    Display* display = canvas->window()->display();
    Coord d0 = display->to_coord(inset);
    Coord d1 = display->to_coord(inset + l1);
    Coord d2 = display->to_coord(inset + l2);
    Coord d3 = display->to_coord(inset + l3);

    canvas->new_path();

    canvas->move_to(l + d1, b + d2);
    canvas->line_to(l + d0, b + d3);
    canvas->line_to(l + d0, t - d3);
    canvas->line_to(l + d1, t - d2);

    canvas->line_to(l + d2, t - d1);
    canvas->line_to(l + d3, t - d0);
    canvas->line_to(r - d3, t - d0);
    canvas->line_to(r - d2, t - d1);

    canvas->line_to(r - d1, t - d2);
    canvas->line_to(r - d0, t - d3);
    canvas->line_to(r - d0, b + d3);
    canvas->line_to(r - d1, b + d2);

    canvas->line_to(r - d2, b + d1);
    canvas->line_to(r - d3, b + d0);
    canvas->line_to(l + d3, b + d0);
    canvas->line_to(l + d2, b + d1);

    canvas->close_path();
}

void OL_ButtonGlyph::top_path(
    Canvas* canvas, int inset, Coord l, Coord b, Coord r, Coord t
) const {
    Display* display = canvas->window()->display();
    Coord d0 = display->to_coord(inset);
    Coord d1 = display->to_coord(inset + l1);
    Coord d2 = display->to_coord(inset + l2);
    Coord d3 = display->to_coord(inset + l3);

    canvas->move_to(l + d1, b + d2);
    canvas->line_to(l + d0, b + d3);
    canvas->line_to(l + d0, t - d3);
    canvas->line_to(l + d1, t - d2);

    canvas->line_to(l + d2, t - d1);
    canvas->line_to(l + d3, t - d0);
    canvas->line_to(r - d3, t - d0);
    canvas->line_to(r - d2, t - d1);
}

void OL_ButtonGlyph::bottom_path(
    Canvas* canvas, int inset, Coord l, Coord b, Coord r, Coord t
) const {
    Display* display = canvas->window()->display();
    Coord d0 = display->to_coord(inset);
    Coord d1 = display->to_coord(inset + l1);
    Coord d2 = display->to_coord(inset + l2);
    Coord d3 = display->to_coord(inset + l3);

    canvas->move_to(r - d1, t - d2);
    canvas->line_to(r - d0, t - d3);
    canvas->line_to(r - d0, b + d3);
    canvas->line_to(r - d1, b + d2);

    canvas->line_to(r - d2, b + d1);
    canvas->line_to(r - d3, b + d0);
    canvas->line_to(l + d3, b + d0);
    canvas->line_to(l + d2, b + d1);
}

/** class OL_AbbrevButtonGlyph **/

OL_AbbrevButtonGlyph::OL_AbbrevButtonGlyph(
    Glyph* label, OL_Style* s, boolean invoked, boolean inactive
) : MonoGlyph(nil) {
    style_ = s;
    Resource::ref(style_);
    invoked_ = invoked;
    inactive_ = inactive;
    const Font* gf = s->glyph_font();
    height_ = gf->ascent(ABBREV_MENU_FILL) + gf->descent(ABBREV_MENU_FILL);
    width_ = gf->width(ABBREV_MENU_FILL);
    body(
        new Margin(label, 0, fil, fil, 0, fil, fil, 0, fil, fil, 0, fil, fil)
    );
}

OL_AbbrevButtonGlyph::~OL_AbbrevButtonGlyph () {
    Resource::unref(style_);
}

void OL_AbbrevButtonGlyph::request(Requisition& requisition) const {
    MonoGlyph::request(requisition);
    Requirement rx(width_);
    Requirement ry(height_);
    requisition.require(Dimension_X, rx);
    requisition.require(Dimension_Y, ry);
}

void OL_AbbrevButtonGlyph::allocate(
    Canvas* canvas, const Allocation& a, Extension& ext
) {
    MonoGlyph::allocate(canvas, a, ext);
    ext.extend(a);
}

void OL_AbbrevButtonGlyph::draw(Canvas* canvas, const Allocation& a) const {
    Coord x = a.x();
    Coord y = a.y() + height_;

    const Font* gf = style_->glyph_font();
    const Color* top, * bottom, * fill;
    if (invoked_) {
	top = style_->dull();
	bottom = style_->bright();
	fill = style_->fill_in();
    } else {
	top = style_->bright();
	bottom = style_->dull();
	fill = style_->fill_out();
    }
    canvas->character(gf, ABBREV_MENU_UL, 10, top, x, y);
    canvas->character(gf, ABBREV_MENU_LR, 10, bottom, x, y);
    canvas->character(gf, ABBREV_MENU_FILL, 10, fill, x, y);
    MonoGlyph::draw(canvas, a);
}

/** OL_ChoiceItem **/

OL_ChoiceItemGlyph::OL_ChoiceItemGlyph(
    Glyph* label, OL_Style* s, boolean invoked, boolean inactive, boolean def
) : MonoGlyph(nil) {
    style_ = s;
    Resource::ref(style_);
    invoked_ = invoked;
    inactive_ = inactive;
    default_ = def;
    body(new Margin(label, 4));
}

OL_ChoiceItemGlyph::~OL_ChoiceItemGlyph() {
    Resource::unref(style_);
}

void OL_ChoiceItemGlyph::allocate(
    Canvas* canvas, const Allocation& a, Extension& ext
) {
    MonoGlyph::allocate(canvas, a, ext);
    ext.extend(a);
}

void OL_ChoiceItemGlyph::draw(Canvas* canvas, const Allocation& a) const {
    Coord l = a.left();
    Coord r = a.right();
    Coord b = a.bottom();
    Coord t = a.top();

    const Color* top, * bottom, * fill, * gray;
    if (invoked_) {
	top = style_->dull();
	bottom = style_->bright();
	fill = style_->fill_in();
	gray = style_->gray_in();
    } else {
	top = style_->bright();
	bottom = style_->dull();
	fill = style_->fill_out();
	gray = style_->gray_out();
    }

    canvas->fill_rect(l, b, r, t, fill);
    MonoGlyph::draw(canvas, a);
    if (inactive_) {
        canvas->fill_rect(l, b, r, t, gray);
    }
    canvas->fill_rect(l, b, l+1, t, top);
    canvas->fill_rect(l, t-1, r, t, top);
    canvas->fill_rect(l, b, r, b+1, bottom);
    canvas->fill_rect(r-1, b, r, t, bottom);
    if (default_) {
	Brush* brush = new Brush(0);
	Resource::ref(brush);
        Coord inset = 2.0;
        canvas->rect(
	    l + inset, b + 1.0 + inset, r - inset - 1.0, t - inset,
	    style_->dull(), brush
	);
	Resource::unref(brush);
    }
}

/** class OL_ChiseledBorder **/

OL_ChiseledBorder::OL_ChiseledBorder(
    Glyph* contents, OL_Style* s
) : MonoGlyph(nil) {
    style_ = s;
    Resource::ref(style_);
    body(contents);
}

OL_ChiseledBorder::~OL_ChiseledBorder() {
    Resource::unref(style_);
}

void OL_ChiseledBorder::allocate(
    Canvas* canvas, const Allocation& a, Extension& ext
) {
    MonoGlyph::allocate(canvas, a, ext);
    ext.extend(a);
}

void OL_ChiseledBorder::draw(Canvas* canvas, const Allocation& a) const {
    Coord l = a.left();
    Coord r = a.right();
    Coord b = a.bottom();
    Coord t = a.top();

    const Color* dull = style_->dull();
    const Color* bright = style_->bright();
    canvas->fill_rect(l, b+1, l+1, t-1, dull);
    canvas->fill_rect(l, t, r-1, t-1, dull);
    canvas->fill_rect(r, t, r-1, b+1, bright);
    canvas->fill_rect(r, b, l, b+1, bright); 
    canvas->fill_rect(l+1, b+2, l+2, t-2, bright);
    canvas->fill_rect(l+1, t-1, r-2, t-2, bright);
    canvas->fill_rect(r-1, t-1, r-2, b+2, dull);
    canvas->fill_rect(r-1, b+1, l+1, b+2, dull);

    MonoGlyph::draw(canvas, a);
}

/** class OL_PushPinGlyph **/

OL_PushPinGlyph::OL_PushPinGlyph(
    OL_Style* s, boolean pushed, boolean inactive
) : Glyph() {
    style_ = s;
    Resource::ref(style_);
    pushed_ = pushed;
    inactive_ = inactive;
    const Font* gf = style_->glyph_font();
    height_ = gf->ascent(PUSHPIN_OUT_BOTTOM) + gf->descent(PUSHPIN_OUT_BOTTOM);
    width_ = gf->width(PUSHPIN_OUT_BOTTOM);
}

OL_PushPinGlyph::~OL_PushPinGlyph() {
    Resource::unref(style_);
}

void OL_PushPinGlyph::request(Requisition& requisition) const {
    Requirement rx(width_);
    Requirement ry(height_);
    requisition.require(Dimension_X, rx);
    requisition.require(Dimension_Y, ry);
}

void OL_PushPinGlyph::allocate(
    Canvas* canvas, const Allocation& a, Extension& ext
) {
    Glyph::allocate(canvas, a, ext);
    ext.extend(a);
}

void OL_PushPinGlyph::draw(Canvas* canvas, const Allocation& a) const {
    Coord x = a.x();
    Coord y = a.y() + height_;
    const Color* top = style_->bright();
    const Color* bottom = style_->dull();
    const Color* fill = style_->fill_in();
    const Font* gf = style_->glyph_font();
    if (pushed_) {
        canvas->character(gf, PUSHPIN_IN_TOP, 10, top, x, y);
        canvas->character(gf, PUSHPIN_IN_BOTTOM, 10, bottom, x, y);
        canvas->character(gf, PUSHPIN_IN_MIDDLE, 10, fill, x, y);
    } else {
        canvas->character(gf, PUSHPIN_OUT_TOP, 10, top, x, y);
        canvas->character(gf, PUSHPIN_OUT_BOTTOM, 10, bottom, x, y);
        canvas->character(gf, PUSHPIN_OUT_MIDDLE, 10, fill, x, y);
    }
}

/** class OL_ElevatorGlyph **/

OL_ElevatorGlyph::OL_ElevatorGlyph() {
    dimpled_ = false;
    abbreviated_ = false;
}

OL_ElevatorGlyph::~OL_ElevatorGlyph() { }

void OL_ElevatorGlyph::set_dimple(boolean b) { dimpled_ = b; }
Coord OL_ElevatorGlyph::length() const { return length_; }
boolean OL_ElevatorGlyph::get_abbreviated() const { return abbreviated_; }

/** class OL_HorizontalElevatorGlyph **/

OL_HorizontalElevatorGlyph::OL_HorizontalElevatorGlyph(
    OL_Style* s,
    boolean abbreviated, boolean left, boolean right, boolean dimpled
) : OL_ElevatorGlyph() {
    style_ = s;
    Resource::ref(style_);
    left_ = left;
    right_ = right;
    dimpled_ = dimpled;
    abbreviated_ = abbreviated;
    const Font* gf = style_->glyph_font();
    int code = abbreviated_ ? HORIZ_ABBREV_SB_UL : HORIZ_SB_UL;
    height_ = gf->ascent(code) + gf->descent(code);
    width_ = gf->width(code);
    length_ = width_;
}

OL_HorizontalElevatorGlyph::~OL_HorizontalElevatorGlyph() {
    Resource::unref(style_);
}

void OL_HorizontalElevatorGlyph::request(Requisition& requisition) const {
    Requirement rx(width_);
    Requirement ry(height_);
    requisition.require(Dimension_X, rx);
    requisition.require(Dimension_Y, ry);
}

void OL_HorizontalElevatorGlyph::allocate(
    Canvas* canvas, const Allocation& a, Extension& ext
) {
    Glyph::allocate(canvas, a, ext);
    ext.extend(a);
}

void OL_HorizontalElevatorGlyph::draw(
    Canvas* canvas, const Allocation& a
) const {
    Coord x = a.x();
    Coord y = a.top();
    const Color* top = style_->bright();
    const Color* bottom = style_->dull();
    const Color* fill = style_->fill_in();
    const Font* gf = style_->glyph_font();

    canvas->fill_rect(x, y - height_, x + width_, y, style_->background());
    if (abbreviated_) {
        canvas->character(gf, HORIZ_ABBREV_SB_UL, 10, top, x, y);
        canvas->character(gf, HORIZ_ABBREV_SB_LR, 10, bottom, x, y);
    } else {
        canvas->character(gf, HORIZ_SB_UL, 10, top, x, y);
        canvas->character(gf, HORIZ_SB_LR, 10, bottom, x, y);
        if (dimpled_) {
            Coord h = gf->descent(DIMPLE_UL);
            Coord w = gf->width(DIMPLE_UL);
            x = a.x() - w*3/4 + width_/2;
            y = a.y() + h*3/4 + height_/2;
            canvas->character(gf, DIMPLE_UL, 10, bottom, x, y);
            canvas->character(gf, DIMPLE_FILL, 10, fill, x, y);
            canvas->character(gf, DIMPLE_LR, 10, top, x, y);
        }
    }
    if (left_) {
        canvas->character(gf, HORIZ_SB_BOX_UL, 10, bottom, x, y);
        canvas->character(gf, HORIZ_SB_LEFTBOX_FILL, 10, fill, x, y);
        canvas->character(gf, HORIZ_SB_BOX_LR, 10, top, x, y);
    }
    if (right_) {
        x = abbreviated_ ? x + width_/2.0 : x + width_*2.0/3.0;
        canvas->character(gf, HORIZ_SB_BOX_UL, 10, bottom, x, y);
        canvas->character(gf, HORIZ_SB_RIGHTBOX_FILL, 10, fill, x, y);
        canvas->character(gf, HORIZ_SB_BOX_LR, 10, top, x, y);
    }
}

void OL_HorizontalElevatorGlyph::set_backward(boolean b) { left_ = b; }
void OL_HorizontalElevatorGlyph::set_forward(boolean b) { right_ = b; }

void OL_HorizontalElevatorGlyph::set_abbreviated(boolean b) {
    if (abbreviated_ != b) {
        abbreviated_ = b;
	const Font* gf = style_->glyph_font();
	int code = abbreviated_ ? HORIZ_ABBREV_SB_UL : HORIZ_SB_UL;
        height_ = gf->ascent(code) + gf->descent(code);
        width_ = gf->width(code);
    }
}

/** class OL_VerticalElevatorGlyph **/

OL_VerticalElevatorGlyph::OL_VerticalElevatorGlyph(
    OL_Style* s, boolean abbreviated, boolean up, boolean down, boolean dimpled
) : OL_ElevatorGlyph() {
    style_ = s;
    Resource::ref(style_);
    up_ = up;
    down_ = down;
    dimpled_ = dimpled;
    abbreviated_ = abbreviated;
    const Font* gf = style_->glyph_font();
    int code = abbreviated_ ? VERT_ABBREV_SB_UL : VERT_SB_UL;
    height_ = gf->ascent(code) + gf->descent(code);
    width_ = gf->width(code);
    length_ = height_;
}

OL_VerticalElevatorGlyph::~OL_VerticalElevatorGlyph() {
    Resource::unref(style_);
}

void OL_VerticalElevatorGlyph::request(Requisition& requisition) const {
//    Requirement rx(width_, 0, 0, 0.5);
//    Requirement ry(height_, 0, 0, 0.5);
    Requirement rx(width_);
    Requirement ry(height_);
    requisition.require(Dimension_X, rx);
    requisition.require(Dimension_Y, ry);
}

void OL_VerticalElevatorGlyph::allocate(
    Canvas* canvas, const Allocation& a, Extension& ext
) {
    Glyph::allocate(canvas, a, ext);
    ext.extend(a);
}

void OL_VerticalElevatorGlyph::draw(
    Canvas* canvas, const Allocation& a
) const {
    Coord x = a.x();
    Coord y = a.y() + height_;
    const Color* top = style_->bright();
    const Color* bottom = style_->dull();
    const Color* fill = style_->fill_in();
    const Font* gf = style_->glyph_font();

    canvas->fill_rect(x, y - height_, x + width_, y, style_->background());
    if (abbreviated_) {
        canvas->character(gf, VERT_ABBREV_SB_UL, 10, top, x, y);
        canvas->character(gf, VERT_ABBREV_SB_LR, 10, bottom, x, y);
    } else {
        canvas->character(gf, VERT_SB_UL, 10, top, x, y);
        canvas->character(gf, VERT_SB_LR, 10, bottom, x, y);
        if (dimpled_) {
            Coord h = gf->descent(DIMPLE_UL);
            Coord w = gf->width(DIMPLE_UL);
	    Coord y = a.y() + height_*2.0/3.0;
            canvas->character(gf, DIMPLE_UL, 10, bottom, x, y);
            canvas->character(gf, DIMPLE_FILL, 10, fill, x, y);
            canvas->character(gf, DIMPLE_LR, 10, top, x, y);
        }
    }
    if (up_) {
        canvas->character(gf, VERT_SB_BOX_UL, 10, bottom, x, y);
        canvas->character(gf, VERT_SB_TOPBOX_FILL, 10, fill, x, y);
        canvas->character(gf, VERT_SB_BOX_LR, 10, top, x, y);
    }
    if (down_) {
        y = abbreviated_ ? y - height_/3.0 : y - height_*2.0/3.0 + 1.0;
        canvas->character(gf,  VERT_SB_BOX_UL, 10, bottom, x, y);
        canvas->character(gf,  VERT_SB_BOTBOX_FILL, 10, fill, x, y);
        canvas->character(gf,  VERT_SB_BOX_LR, 10, top, x, y);
    }
}

void OL_VerticalElevatorGlyph::set_forward(boolean b) { up_ = b; }
void OL_VerticalElevatorGlyph::set_backward(boolean b) { down_ = b; }

void OL_VerticalElevatorGlyph::set_abbreviated(boolean b) {
    if (abbreviated_ != b) {
        abbreviated_ = b;
	const Font* gf = style_->glyph_font();
	int code = abbreviated_ ? VERT_ABBREV_SB_UL : VERT_SB_UL;
        height_ = gf->ascent(code) + gf->descent(code);
        width_ = gf->width(code);
    }
}

/** class OL_CheckBoxGlyph **/

OL_CheckBoxGlyph::OL_CheckBoxGlyph(
    OL_Style* s, boolean checked, boolean invoked, boolean inactive
) : Glyph() {
    style_ = s;
    Resource::ref(style_);
    checked_ = checked;
    invoked_ = invoked;
    inactive_ = inactive;
    const Font* gf = style_->glyph_font();
    height_ = gf->ascent(UNCHECKED_BOX_UL) + gf->descent(UNCHECKED_BOX_UL);
    width_ = gf->width(UNCHECKED_BOX_UL);
}

OL_CheckBoxGlyph::~OL_CheckBoxGlyph() {
    Resource::unref(style_);
}

void OL_CheckBoxGlyph::request(Requisition& requisition) const {
    Requirement rx(width_);
    Requirement ry(height_);
    requisition.require(Dimension_X, rx);
    requisition.require(Dimension_Y, ry);
}

void OL_CheckBoxGlyph::allocate(
    Canvas* canvas, const Allocation& a, Extension& ext
) {
    Glyph::allocate(canvas, a, ext);
    ext.extend(a);
}

void OL_CheckBoxGlyph::draw(Canvas* canvas, const Allocation& a) const {
    Coord x = a.x();
    Coord y = a.y();

    const Color* top, * bottom, * fill;
    if (invoked_) {
	top = style_->dull();
	bottom = style_->bright();
	fill = style_->fill_in();
    } else {
	top = style_->bright();
	bottom = style_->dull();
	fill = style_->fill_out();
    }
    const Font* gf = style_->glyph_font();
    canvas->character(gf, UNCHECKED_BOX_UL, 10, top, x, y + height_);
    canvas->character(gf, UNCHECKED_BOX_LR, 10, bottom, x, y + height_);
    if (checked_) {
        canvas->character(gf, CHECK_MARK, 10, style_->dull(), x, y + height_);
        canvas->character(gf, CHECKED_BOX_FILL, 10, fill, x, y + height_);
    } else {
        canvas->character(
	    gf, CHECK_BOX_CLEAR_FILL, 10, style_->fill_out(), x, y + height_
	);
        canvas->character(gf, UNCHECKED_BOX_FILL, 10, fill, x, y + height_); 
    }
}

/** class OL_VerticalMenuMark **/

OL_VerticalMenuMarkGlyph::OL_VerticalMenuMarkGlyph(OL_Style* s) : Glyph() {
    style_ = s;
    Resource::ref(style_);
    const Font* gf = style_->glyph_font();
    height_ =
        gf->ascent(VERT_MENU_MARK_FILL) + gf->descent(VERT_MENU_MARK_FILL);
    width_ = gf->width(VERT_MENU_MARK_FILL);
}

OL_VerticalMenuMarkGlyph::~OL_VerticalMenuMarkGlyph() {
    Resource::unref(style_);
}

void OL_VerticalMenuMarkGlyph::request(Requisition& requisition) const {
    Requirement rx(width_);
    Requirement ry(height_);
    requisition.require(Dimension_X, rx);
    requisition.require(Dimension_Y, ry);
}

void OL_VerticalMenuMarkGlyph::allocate(
    Canvas*, const Allocation& a, Extension& ext
) {
    ext.extend(a);
}

void OL_VerticalMenuMarkGlyph::draw(
    Canvas* canvas, const Allocation& a
) const {
    Coord x = a.x();
    Coord y = a.y() + height_;
    const Font* gf = style_->glyph_font();
    canvas->character(gf, VERT_MENU_MARK_UL, 10, style_->dull(), x, y);
    canvas->character(gf, VERT_MENU_MARK_LR, 10, style_->bright(), x, y);
    canvas->character(gf, VERT_MENU_MARK_FILL, 10, style_->fill_in(), x, y);
}

/** class OL_HorizontalMenuMarkGlyph **/

OL_HorizontalMenuMarkGlyph::OL_HorizontalMenuMarkGlyph(OL_Style* s) : Glyph() {
    style_ = s;
    Resource::ref(style_);
    const Font* gf = style_->glyph_font();
    height_ =
	gf->ascent(HORIZ_MENU_MARK_FILL) + gf->descent(HORIZ_MENU_MARK_FILL);
    width_ = gf->width(HORIZ_MENU_MARK_FILL);
}

OL_HorizontalMenuMarkGlyph::~OL_HorizontalMenuMarkGlyph() {
    Resource::unref(style_);
}

void OL_HorizontalMenuMarkGlyph::request(Requisition& requisition) const {
    Requirement rx(width_);
    Requirement ry(height_);
    requisition.require(Dimension_X, rx);
    requisition.require(Dimension_Y, ry);
}

void OL_HorizontalMenuMarkGlyph::allocate(
    Canvas*, const Allocation& a, Extension& ext
) {
    ext.extend(a);
}

void OL_HorizontalMenuMarkGlyph::draw(
    Canvas* canvas, const Allocation& a
) const {
    Coord x = a.x();
    Coord y = a.y() + height_;

    const Font* gf = style_->glyph_font();

    canvas->character(gf, HORIZ_MENU_MARK_UL, 10, style_->dull(), x, y);
    canvas->character(gf, HORIZ_MENU_MARK_LR, 10, style_->bright(), x, y);
    canvas->character(gf, HORIZ_MENU_MARK_FILL, 10, style_->fill_in(), x, y);
}

/** class OL_DimpleGlyph **/

OL_DimpleGlyph::OL_DimpleGlyph(OL_Style* s, boolean in) : Glyph() {
    style_ = s;
    Resource::ref(style_);
    in_ = in;
    const Font* gf = style_->glyph_font();
    height_ = gf->ascent(DIMPLE_LR) + gf->descent(DIMPLE_LR);
    width_ = gf->width(DIMPLE_LR);
}

OL_DimpleGlyph::~OL_DimpleGlyph() {
    Resource::unref(style_);
}

void OL_DimpleGlyph::request(Requisition& requisition) const {
    Requirement rx(width_);
    Requirement ry(height_);
    requisition.require(Dimension_X, rx);
    requisition.require(Dimension_Y, ry);
}

void OL_DimpleGlyph::allocate(
    Canvas*, const Allocation& a, Extension& ext
) {
    ext.extend(a);
}

void OL_DimpleGlyph::draw(Canvas* canvas, const Allocation& a) const {
    const Color* top, * bottom, * fill;
    if (in_) {
	top = style_->dull();
	bottom = style_->bright();
	fill = style_->fill_in();
    } else {
	top = style_->bright();
	bottom = style_->dull();
	fill = style_->fill_out();
    }
    const Font* gf = style_->glyph_font();
    Coord x = a.x();
    Coord y = a.y() + height_;
    canvas->character(gf, DIMPLE_UL, 10, top, x, y);
    canvas->character(gf, DIMPLE_FILL, 10, fill, x, y);
    canvas->character(gf, DIMPLE_LR, 10, bottom, x, y);
}

void OL_DimpleGlyph::set_in(boolean b) { in_ = b; }

/** class OL_AnchorGlyph **/

OL_AnchorGlyph::OL_AnchorGlyph(OL_Style* s, boolean in) : Glyph() {
    style_ = s;
    in_ = in;
    Resource::ref(style_);
}

OL_AnchorGlyph::~OL_AnchorGlyph() {
    Resource::unref(style_);
}

void OL_AnchorGlyph::request(Requisition& requisition) const {
    Requirement rx(width_);
    Requirement ry(height_);
    requisition.require(Dimension_X, rx);
    requisition.require(Dimension_Y, ry);
}

void OL_AnchorGlyph::allocate(
    Canvas*, const Allocation& a, Extension& ext
) {
    ext.extend(a);
}

void OL_AnchorGlyph::set_in(boolean b) { in_ = b; }

/** class OL_LRAnchorGlyph **/

OL_LRAnchorGlyph::OL_LRAnchorGlyph(
    OL_Style* s, boolean in
) : OL_AnchorGlyph(s, in) {
    const Font* gf = style_->glyph_font();
    height_ = gf->ascent(OLG_HSB_ANCHOR) + gf->descent(OLG_HSB_ANCHOR);
    width_ = gf->width(OLG_HSB_ANCHOR);
}

OL_LRAnchorGlyph::~OL_LRAnchorGlyph() { }

void OL_LRAnchorGlyph::draw(Canvas* canvas, const Allocation& a) const {
    const Color* fill = in_ ? style_->fill_in() : style_->fill_out();
    const Font* gf = style_->glyph_font();
    Coord l = a.left();
    Coord b = a.bottom();
    Coord r = a.right();
    Coord t = a.top();
    Coord x = a.x();
    Coord y = a.y() + height_;
    Brush* brush = new Brush(0);
    Resource::ref(brush);
    if (in_) {
        canvas->character(gf, OLG_HSB_ANCHOR_INVERTED, 10, fill, x, y);
        canvas->new_path();
        canvas->move_to(l, b + 1.0);
        canvas->stroke(style_->bright(), brush);
        canvas->line_to(l, t);
        canvas->line_to(r - 1.0, t);
        canvas->close_path();
        canvas->new_path();
        canvas->stroke(style_->dull(), brush);
        canvas->line_to(r - 1.0, b + 1.0);
        canvas->line_to(l, b + 1.0);
        canvas->close_path();
    } else {
        canvas->character(gf, OLG_HSB_ANCHOR, 10, fill, x, y);
        canvas->move_to(l, b + 1.0);
        canvas->stroke(style_->dull(), brush);
        canvas->line_to(l, t);
        canvas->line_to(r - 1.0, t);
        canvas->stroke(style_->bright(), brush);
        canvas->line_to(r - 1.0, b + 1.0);
        canvas->line_to(l, b + 1.0);
    }
    Resource::unref(brush);
}

OL_TBAnchorGlyph::OL_TBAnchorGlyph(
    OL_Style* s, boolean in
) : OL_AnchorGlyph(s,in) {
    const Font* gf = style_->glyph_font();
    height_ = gf->ascent(OLG_VSB_ANCHOR) + gf->descent(OLG_VSB_ANCHOR);
    width_ = gf->width(OLG_VSB_ANCHOR);
}

OL_TBAnchorGlyph::~OL_TBAnchorGlyph() { }

void OL_TBAnchorGlyph::draw(Canvas* canvas, const Allocation& a) const {
    const Font* gf = style_->glyph_font();
    Coord x = a.x();
    Coord y = a.y() + height_;
    if (in_) {
        canvas->character(
	    gf, OLG_VSB_ANCHOR_INVERTED, 10, style_->fill_in(), x, y
	);
    } else {
        canvas->character(gf, OLG_VSB_ANCHOR, 10, style_->fill_out(), x, y);
    }
}

/** class OL_TitleBarGlyph **/

OL_TitleBarGlyph::OL_TitleBarGlyph(OL_Style* s, Coord margin) : Glyph() {
    style_ = s;
    Resource::ref(style_);
    margin_ = margin;
}

OL_TitleBarGlyph::~OL_TitleBarGlyph() {
    Resource::unref(style_);
}

void OL_TitleBarGlyph::request(Requisition& requisition) const {
    Requirement ry(2.0*margin_ + 2.0);
    requisition.require(Dimension_Y, ry);
}

void OL_TitleBarGlyph::draw(Canvas* canvas, const Allocation& a) const {
    Coord l = a.left();
    Coord r = a.right();
    Coord y = a.top() - margin_;
    const Color* bright = style_->bright();
    const Color* dull = style_->dull();
    Brush* brush = new Brush(1.0);
    Resource::ref(brush);
    canvas->line(l, y, r, y, bright, brush);
    canvas->line(l, y - 1.0, r, y - 1.0, dull, brush);
    canvas->line(l, y - 1.0, l + 1.0, y - 1.0, bright, brush);
    canvas->line(r, y, r - 1.0, y, dull, brush);
    Resource::unref(brush);
}
