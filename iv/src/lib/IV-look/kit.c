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
 * Kit -- object for creating common UI objects
 */

#include <IV-look/mf_kit.h>
#include <IV-look/mono_kit.h>
#include <IV-look/ol_kit.h>
#include <InterViews/color.h>
#include <InterViews/session.h>
#include <InterViews/style.h>

class KitImpl {
private:
    friend class Kit;

    static Kit* instance_;

    static Kit* make_kit();
};

Kit* KitImpl::instance_;

Kit::Kit() { }
Kit::~Kit() { }

Kit* Kit::instance() {
    if (KitImpl::instance_ == nil) {
	KitImpl::instance_ = KitImpl::make_kit();
    }
    return KitImpl::instance_;
}

Kit* KitImpl::make_kit() {
    Style* s = Session::instance()->style();
    if (s->value_is_on("monochrome") || s->value_is_on("Monochrome")) {
	return new MonoKit;
    }

    const Color* c1 = new Color(0.0, 0.0, 0.0, 1.0);
    const Color* c2 = new Color(1.0, 1.0, 1.0, 1.0);
    const Color* c3 = new Color(0.5, 0.5, 0.5, 1.0);
    if (!c1->distinguished(c2) || !c2->distinguished(c3)) {
	return new MonoKit;
    }

    if (s->value_is_on("OpenLook") || s->value_is_on("openlook")) {
	return new OLKit;
    }
    return new MFKit;
}
