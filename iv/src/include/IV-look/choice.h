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

#ifndef ivlook_choice_h
#define ivlook_choice_h

/*
 * ChoiceItem - telltale with different look for each state
 */

#include <IV-look/telltale.h>

class ChoiceItem : public Telltale {
public:
    ChoiceItem();
    ChoiceItem(Glyph* normal, Glyph* pressed);
    virtual ~ChoiceItem();

    enum {
	is_enabled = 0x1, is_inside = 0x2, is_pressed = 0x4, is_chosen = 0x8
    };

    unsigned int numlooks() const;
    boolean contains_looks(unsigned int set, unsigned int subset) const;

    virtual void look(unsigned int bitset, Glyph*);
    virtual Glyph* look(unsigned int bitset) const;

    virtual void highlight(boolean);
    virtual void choose(boolean);
    virtual void enable(boolean);
private:
    class Deck* deck_;
    unsigned int index_;

    void init();
    void update(unsigned int flag, boolean b);
};

/*
 * This function needs to be changed is the set of possible looks changes
 * (enum above).
 */

inline unsigned int ChoiceItem::numlooks() const {
    return is_enabled + is_inside + is_pressed + is_chosen;
}

#endif
