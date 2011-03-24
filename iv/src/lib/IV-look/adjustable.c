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
 * Adjustable - object to scroll and/or zoom
 */

#include <IV-look/adjustable.h>
#include <IV-look/adjuster.h>
#include <OS/list.h>

declareList(AdjusterList,Adjuster*);
implementList(AdjusterList,Adjuster*);

Adjustable::Adjustable() {
    views_ = new AdjusterList;
}

Adjustable::~Adjustable() {
    for (int i = 0; i < views_->count(); i++) {
	views_->item(i)->disconnect(this);
    }
    delete views_;
}

void Adjustable::attach(Adjuster* a) {
    views_->append(a);
}

void Adjustable::detach(Adjuster* a) {
    for (int i = 0; i < views_->count(); i++) {
	if (a == views_->item(i)) {
	    views_->remove(i);
	    break;
	}
    }
}

void Adjustable::scroll_forward(DimensionName) { }
void Adjustable::scroll_backward(DimensionName) { }
void Adjustable::page_forward(DimensionName) { }
void Adjustable::page_backward(DimensionName) { }
void Adjustable::scroll_to(DimensionName, Coord) { }
void Adjustable::scale_to(DimensionName, Coord) { }
void Adjustable::zoom_to(float) { }

void Adjustable::constrain(DimensionName d, Coord& position) const {
    Coord a = lower(d), b = upper(d) - cur_length(d);
    if (position < a) {
	position = a;
    } else if (position > b) {
	position = b;
    }
}

void Adjustable::notify() const {
    for (int i = 0; i < views_->count(); i++) {
	views_->item(i)->update_all();
    }
}

void Adjustable::notify(DimensionName d) const {
    for (int i = 0; i < views_->count(); i++) {
	views_->item(i)->update(d);
    }
}
