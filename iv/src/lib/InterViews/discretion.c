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

#include <InterViews/discretion.h>

Discretionary::Discretionary(int penalty, Glyph* glyph) : Glyph() {
    penalty_ = penalty;
    nobreak_ = glyph;
    if (nobreak_ != nil) {
        nobreak_->ref();
    }
    prebreak_ = glyph;
    if (prebreak_ != nil) {
        prebreak_->ref();
    }
    inbreak_ = nil;
    postbreak_ = nil;
}

Discretionary::Discretionary(
    int penalty, Glyph* no, Glyph* pre, Glyph*in, Glyph* post
) : Glyph() {
    penalty_ = penalty;
    nobreak_ = no;
    if (nobreak_ != nil) {
        nobreak_->ref();
    }
    prebreak_ = pre;
    if (prebreak_ != nil) {
        prebreak_->ref();
    }
    inbreak_ = in;
    if (inbreak_ != nil) {
        inbreak_->ref();
    }
    postbreak_ = post;
    if (postbreak_ != nil) {
        postbreak_->ref();
    }
}

Discretionary::~Discretionary() {
    if (nobreak_ != nil) {
        nobreak_->unref();
    }
    if (prebreak_ != nil) {
        prebreak_->unref();
    }
    if (inbreak_ != nil) {
        inbreak_->unref();
    }
    if (postbreak_ != nil) {
        postbreak_->unref();
    }
}    

void Discretionary::request(Requisition& requisition) const {
    if (nobreak_ != nil) {
        nobreak_->request(requisition);
    }
    requisition.penalty(penalty_);
}

Glyph* Discretionary::compose(GlyphBreakType b) {
    if (b == no_break) {
        return nobreak_;
    } else if (b == pre_break) {
        return prebreak_;
    } else if (b == in_break) {
        return inbreak_;
    } else if (b == post_break) {
        return postbreak_;
    } else {
        return nil;
    }
}
