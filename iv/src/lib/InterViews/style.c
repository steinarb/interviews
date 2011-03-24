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
 * Style - style information
 */

#include <InterViews/brush.h>
#include <InterViews/color.h>
#include <InterViews/display.h>
#include <InterViews/font.h>
#include <InterViews/session.h>
#include <InterViews/style.h>
#include <OS/list.h>
#include <OS/table.h>
#include <OS/ustring.h>
#include <ctype.h>

declareList(StyleList,Style*);
implementList(StyleList,Style*);

declareList(UniqueStringList,UniqueString*);
implementList(UniqueStringList,UniqueString*);

static void delete_UniqueStringList(UniqueStringList* list) {
    if (list != nil) {
	for (ListItr(UniqueStringList) i(*list); i.more(); i.next()) {
	    UniqueString* s = i.cur();
	    delete s;
	}
	delete list;
    }
}

class StyleAttribute {
public:
    StyleAttribute(
	const UniqueString& name, const String& value, int priority
    );
    StyleAttribute(const StyleAttribute&, boolean cached = true);
    virtual ~StyleAttribute();

    const UniqueString& name() const;
    const String& value() const;
    int priority() const;

    virtual boolean match(const UniqueString&, String&) const;
    virtual boolean update(const UniqueString&, const String&, int priority);
    virtual boolean is_cached() const;

    static String* parse_value(const String&);
private:
    UniqueString name_;
    String* value_;
    int priority_;

    /* not permitted */
    void operator =(const StyleAttribute&);
};

StyleAttribute::StyleAttribute(const UniqueString& n, const String& v, int p) :
    name_(n), value_(parse_value(v)), priority_(p) { }

StyleAttribute::StyleAttribute(const StyleAttribute& a, boolean cached) :
    name_(a.name_), value_(new String(*a.value_)),
    priority_(cached ? -1000 : a.priority_) { }

StyleAttribute::~StyleAttribute() {
    delete value_;
}

inline const UniqueString& StyleAttribute::name() const { return name_; }
inline const String& StyleAttribute::value() const { return *value_; }
inline int StyleAttribute::priority() const { return priority_; }

boolean StyleAttribute::match(const UniqueString& n, String& v) const {
    if (name_ == n) {
	v = *value_;
	return true;
    }
    return false;
}

boolean StyleAttribute::update(const UniqueString& n, const String& v, int p) {
    if (name_ == n) {
	if (p >= priority_) {
	    value_ = parse_value(v);
	    priority_ = p;
	}
	return true;
    }
    return false;
}

boolean StyleAttribute::is_cached() const {
    return priority_ == -1000;
}

/*
 * Scan an attribute value, replace \<char> as follows:
 *
 *	\<newline>	nothing
 *	\n		newline
 *	\\		\
 */

class ValueString : public String {
public:
    ValueString(char*, int len);
    virtual ~ValueString();

    virtual boolean null_terminated() const;
};

ValueString::ValueString(char* str, int len) : String(str, len) { }
ValueString::~ValueString() {
    char* s = (char*)string();
    delete [] s;
}

boolean ValueString::null_terminated() const { return true; }

String* StyleAttribute::parse_value(const String& v) {
    if (v.index('\\') == -1) {
	return new NullTerminatedString(v);
    }
    const char* src = v.string();
    int len = v.length();
    const char* src_end = src + len;
    char* dst_start = new char[len + 1];
    char* dst = dst_start;
    for (; src < src_end; src++) {
	if (*src == '\\') {
	    ++src;
	    switch (*src) {
	    case '\n':
		/* eliminate */
		break;
	    case 'n':
		*dst++ = '\n';
		break;
	    default:
		*dst++ = *src;
		break;
	    }
	} else {
	    *dst++ = *src;
	}
    }
    *dst = '\0';
    return new ValueString(dst_start, dst - dst_start);
}

class StyleWildcardInfo {
public:
    long depth_;
    long index_;
};

declareList(StyleWildcardInfoList,StyleWildcardInfo);
implementList(StyleWildcardInfoList,StyleWildcardInfo);

class StyleWildcardMatchQuality {
public:
    StyleWildcardMatchQuality();
    virtual ~StyleWildcardMatchQuality();

    void zero();
    void push(long element, long index);
    void pop();
    boolean operator >(StyleWildcardMatchQuality& q);
    void operator =(const StyleWildcardMatchQuality& q);
    boolean defined() const;
private:
    StyleWildcardInfoList list_;
};

StyleWildcardMatchQuality::StyleWildcardMatchQuality() : list_(10) { }
StyleWildcardMatchQuality::~StyleWildcardMatchQuality() { }

inline void StyleWildcardMatchQuality::zero() { list_.remove_all(); }

void StyleWildcardMatchQuality::push(long element, long index) {
    StyleWildcardInfo info;
    info.depth_ = element;
    info.index_ = index;
    list_.prepend(info);
}

inline void StyleWildcardMatchQuality::pop() {
    list_.remove(list_.count() - 1);
}

boolean StyleWildcardMatchQuality::operator >(
    StyleWildcardMatchQuality& q
) {
    ListItr(StyleWildcardInfoList) i(list_);
    ListItr(StyleWildcardInfoList) q_i(q.list_);
    for (; i.more(); i.next(), q_i.next()) {
	if (!q_i.more()) {
	    return true;
	}
	const StyleWildcardInfo& info = i.cur();
	const StyleWildcardInfo& q_info = q_i.cur();
	if (info.depth_ > q_info.depth_) {
	    return true;
	}
	if (info.depth_ < q_info.depth_) {
	    return false;
	}
	if (info.index_ < q_info.index_) {
	    return true;
	}
	if (info.index_ > q_info.index_) {
	    return false;
	}
    }
    return !q_i.more();
}

void StyleWildcardMatchQuality::operator =(
    const StyleWildcardMatchQuality& q
) {
    list_.remove_all();
    for (ListItr(StyleWildcardInfoList) i(q.list_); i.more(); i.next()) {
	list_.append(i.cur_ref());
    }
}

inline boolean StyleWildcardMatchQuality::defined() const {
    return list_.count() != 0;
}

declareList(AttributeList,StyleAttribute*);
implementList(AttributeList,StyleAttribute*);

class StyleWildcard : public StyleAttribute {
public:
    StyleWildcard(
	UniqueStringList* path, const UniqueString& s, const String& v, int p
    );
    StyleWildcard(const StyleWildcard&, boolean cached = true);
    ~StyleWildcard();

    void wildcard_match(
	StyleList&, const UniqueString& name,
	StyleWildcardMatchQuality&, String& v
    ) const;
    boolean wildcard_update(
	UniqueStringList& path, const UniqueString& s, const String& v, int p
    );
private:
    UniqueStringList* path_;
};

StyleWildcard::StyleWildcard(
    UniqueStringList* path, const UniqueString& name, const String& v, int p
) : StyleAttribute(name, v, p), path_(path) { }

StyleWildcard::StyleWildcard(
    const StyleWildcard& s, boolean cached
) : StyleAttribute(s, cached) {
    path_ = new UniqueStringList;
    for (ListItr(UniqueStringList) i(*s.path_); i.more(); i.next()) {
	UniqueString* u = new UniqueString(*i.cur());
	path_->append(u);
    }
}

StyleWildcard::~StyleWildcard() {
    delete_UniqueStringList(path_);
}

boolean StyleWildcard::wildcard_update(
    UniqueStringList& path, const UniqueString& s, const String& v, int p
) {
    if (path.count() == path_->count()) {
	ListItr(UniqueStringList) i(path);
	ListItr(UniqueStringList) j(*path_);
	for (; i.more(); i.next(), j.next()) {
	    if (*i.cur() != *j.cur()) {
		return false;
	    }
	}
	return update(s, v, p);
    }
    return false;
}

declareList(WildcardList,StyleWildcard*);
implementList(WildcardList,StyleWildcard*);

declareTable(StyleToFont,StyleRep*,const Font*);
implementTable(StyleToFont,StyleRep*,const Font*);

declareTable(StyleToColor,StyleRep*,const Color*);
implementTable(StyleToColor,StyleRep*,const Color*);

struct FancyColors {
    const Color* flat;
    const Color* light;
    const Color* dull;
    const Color* dark;
};

declareTable(StyleToFancyColors,StyleRep*,FancyColors*);
implementTable(StyleToFancyColors,StyleRep*,FancyColors*);

class StyleRep {
    friend class Style;
    friend class StyleWildcard;

    StyleRep(UniqueString*, Style*);
    ~StyleRep();

    void clear_info();
    void modify();
    void update();
    void copy_attributes(AttributeList&);
    void copy_wildcards(WildcardList&);
    void copy_prefixes(UniqueStringList&);

    boolean parse_name(String&, int& priority, UniqueStringList*&);
    int find_separator(const String&);
    boolean match_name(const UniqueString&, int& priority);

    boolean find_normal(const UniqueString& name, String& value);
    boolean find_wildcard(StyleList&, const UniqueString& name, String& value);

    static long prefix_index(Style*, const UniqueString&);

    const Color* find_color(
	const Style*, StyleToColor* table,
	const UniqueString* s1, const UniqueString* s2
    );
    boolean valid_font(const String&, StyleRep*, const Font*&);
    boolean valid_color(
	const String&, StyleRep*, StyleToColor* table, const Color*&
    );

    UniqueString* name_;
    UniqueStringList* prefixes_;
    Style* parent_;
    AttributeList* values_;
    WildcardList* wildcard_values_;
    StyleList* children_;
    boolean modified_;

    static StyleToFont* font_table_;
    static StyleToColor* foreground_table_;
    static StyleToColor* background_table_;
    static StyleToFancyColors* fancy_table_;

    static UniqueString* font_string_;
    static UniqueString* Font_string_;
    static UniqueString* foreground_string_;
    static UniqueString* Foreground_string_;
    static UniqueString* background_string_;
    static UniqueString* Background_string_;
    static UniqueString* flat_string_;
    static UniqueString* Flat_string_;
};

StyleToFont* StyleRep::font_table_;
StyleToColor* StyleRep::foreground_table_;
StyleToColor* StyleRep::background_table_;
StyleToFancyColors* StyleRep::fancy_table_;

UniqueString* StyleRep::font_string_;
UniqueString* StyleRep::Font_string_;
UniqueString* StyleRep::foreground_string_;
UniqueString* StyleRep::Foreground_string_;
UniqueString* StyleRep::background_string_;
UniqueString* StyleRep::Background_string_;
UniqueString* StyleRep::flat_string_;
UniqueString* StyleRep::Flat_string_;

StyleRep::StyleRep(UniqueString* s, Style* p) {
    name_ = s;
    parent_ = p;
    children_ = nil;
    prefixes_ = nil;
    values_ = new AttributeList;
    wildcard_values_ = nil;
    modified_ = true;

    if (font_table_ == nil) {
	font_table_ = new StyleToFont(128);
	foreground_table_ = new StyleToColor(256);
	background_table_ = new StyleToColor(256);
	fancy_table_ = new StyleToFancyColors(16);

	font_string_ = new UniqueString("font");
	Font_string_ = new UniqueString("Font");
	foreground_string_ = new UniqueString("foreground");
	Foreground_string_ = new UniqueString("Foreground");
	background_string_ = new UniqueString("background");
	Background_string_ = new UniqueString("Background");
	flat_string_ = new UniqueString("flat");
	Flat_string_ = new UniqueString("Flat");
    }
}

StyleRep::~StyleRep() {
    delete name_;

    for (ListItr(AttributeList) i(*values_); i.more(); i.next()) {
	StyleAttribute* a = i.cur();
	delete a;
    }
    delete values_;

    if (wildcard_values_ != nil) {
	for (ListItr(WildcardList) i(*wildcard_values_); i.more(); i.next()) {
	    StyleWildcard* s = i.cur();
	    delete s;
	}
	delete wildcard_values_;
    }

    delete_UniqueStringList(prefixes_);

    if (children_ != nil) {
	for (ListItr(StyleList) i(*children_); i.more(); i.next()) {
	    Style* s = i.cur();
	    s->rep_->parent_ = nil;
	    Resource::unref(s);
	}
	delete children_;
    }
}

Style::Style() {
    rep_ = new StyleRep(nil, nil);
}

Style::Style(const String& name) {
    rep_ = new StyleRep(new UniqueString(name), nil);
}

Style::Style(Style* p) {
    rep_ = new StyleRep(nil, p);
    p->append(this);
}

Style::Style(const String& name, Style* p) {
    rep_ = new StyleRep(new UniqueString(name), p);
    p->append(this);
}

Style::Style(const Style& style) {
    StyleRep& s = *style.rep_;
    UniqueString* name;
    if (s.name_ != nil) {
	name = new UniqueString(*s.name_);
    } else {
	name = nil;
    }
    rep_ = new StyleRep(name, s.parent_);
    if (s.parent_ != nil) {
	s.parent_->append(this);
    }
    s.update();
    rep_->copy_attributes(*s.values_);
    if (s.wildcard_values_ != nil) {
	rep_->copy_wildcards(*s.wildcard_values_);
    }
    if (s.prefixes_ != nil) {
	rep_->copy_prefixes(*s.prefixes_);
    }
    rep_->modify();
}

Style::~Style() {
    delete rep_;
}

const String& Style::name() const {
    static UniqueString* default_name = nil;
    UniqueString* s = rep_->name_;
    if (s == nil) {
	if (default_name == nil) {
	    default_name = new UniqueString("Unknown style");
	}
	s = default_name;
    }
    return *s;
}

Style* Style::parent() const {
    return rep_->parent_;
}

void Style::name(const String& str) {
    StyleRep* s = rep_;
    delete s->name_;
    s->name_ = new UniqueString(str);
}

/*
 * Add a prefix to the current style.
 */

void Style::prefix(const String& name) {
    UniqueString* s = new UniqueString(name);
    UniqueStringList* list = rep_->prefixes_;
    if (list == nil) {
	list = new UniqueStringList(5);
	rep_->prefixes_ = list;
    }
    list->prepend(s);
    rep_->modify();
}

/*
 * Add a child style.  Implicitly remove the child from its current parent,
 * if it has one, and set its parent to this.  Because reparenting may change
 * attributes, we must mark the child modified.
 */

void Style::append(Style* style) {
    Style* p = style->parent();
    if (p == this) {
	return;
    }

    Resource::ref(style);

    if (p != nil) {
	p->remove(style);
    }

    StyleList* list = rep_->children_;
    if (list == nil) {
	list = new StyleList;
	rep_->children_ = list;
    }
    list->append(style);

    StyleRep* s = style->rep_;
    s->parent_ = this;
    s->modify();
}

/*
 * Remove a child style.  Do nothing the given style isn't really a child.
 */

void Style::remove(Style* style) {
    if (rep_->children_ != nil) {
	for (ListItr(StyleList) i(*rep_->children_); i.more(); i.next()) {
	    if (i.cur() == style) {
		i.remove_cur();
		style->rep_->parent_ = nil;
		Resource::unref(style);
		break;
	    }
	}
    }
}

/*
 * Return the number of children styles.
 */

long Style::children() const {
    StyleList* s = rep_->children_;
    return s == nil ? 0 : s->count();
}

/*
 * Return a particular child.
 */

Style* Style::child(long i) const {
    StyleList* s = rep_->children_;
    if (s != nil && i >= 0 && i < s->count()) {
	return s->item(i);
    }
    return nil;
}

/*
 * Find a child style with the given name.
 */

Style* Style::find_style(const String& name) const {
    if (rep_->children_ != nil) {
	UniqueString u(name);
	for (ListItr(StyleList) i(*rep_->children_); i.more(); i.next()) {
	    Style* s = i.cur();
	    StyleRep* sr = s->rep_;
	    if (sr->name_ != nil && *sr->name_ == u) {
		return s;
	    }
	}
    }
    return nil;
}

/*
 * Copy attributes from another style.
 */

void StyleRep::copy_attributes(AttributeList& list) {
    for (ListItr(AttributeList) i(list); i.more(); i.next()) {
	StyleAttribute* a = new StyleAttribute(*i.cur(), false);
	values_->append(a);
    }
}

/*
 * Copy wildcard attributes from another style.
 */

void StyleRep::copy_wildcards(WildcardList& list) {
    wildcard_values_ = new WildcardList;
    for (ListItr(WildcardList) i(list); i.more(); i.next()) {
	StyleWildcard* w = new StyleWildcard(*i.cur(), false);
	wildcard_values_->append(w);
    }
}

/*
 * Copy prefixes from another style.
 */

void StyleRep::copy_prefixes(UniqueStringList& list) {
    prefixes_ = new UniqueStringList(5);
    for (ListItr(UniqueStringList) i(list); i.more(); i.next()) {
	UniqueString* u = new UniqueString(*i.cur());
	prefixes_->append(u);
    }
}

/*
 * Put a <name, value> pair on the attribute list.
 */

void Style::attribute(const String& name, const String& value, int priority) {
    StyleRep* s = rep_;
    String str(name);
    int p = priority;

    UniqueStringList* pathlist;
    if (!s->parse_name(str, p, pathlist)) {
	/* irrelevant attribute: A*B where A doesn't match */
	return;
    }

    UniqueString u(str);
    if (pathlist == nil) {
	/* simple name */
	for (ListItr(AttributeList) i(*s->values_); i.more(); i.next()) {
	    if (i.cur()->update(u, value, p)) {
		return;
	    }
	}
	StyleAttribute* a = new StyleAttribute(u, value, p);
	s->values_->append(a);
    } else {
	/* name with wildcarding */
	WildcardList* wc = s->wildcard_values_;
	if (wc == nil) {
	    wc = new WildcardList;
	    s->wildcard_values_ = wc;
	}
	for (ListItr(WildcardList) i(*wc); i.more(); i.next()) {
	    if (i.cur()->wildcard_update(*pathlist, u, value, p)) {
		delete_UniqueStringList(pathlist);
		return;
	    }
	}
	StyleWildcard* w = new StyleWildcard(pathlist, u, value, p);
	wc->append(w);
    }
    s->modify();
}

/*
 * Parse a name of the form *A*B*C into the list of names A, B, C.
 * Strip the first name (e.g., A) if it matches the style's name
 * or a prefix.
 */

boolean StyleRep::parse_name(
    String& s, int& priority, UniqueStringList*& list
) {
    boolean leading_star = false;
    if (s[0] == '*') {
	leading_star = true;
	s.set_to_right(1);
    }

    list = nil;
    boolean first = true;
    for (int i = find_separator(s); i != -1; i = find_separator(s)) {
	UniqueString name(s.left(i));
	if (first) {
	    first = false;
	    if (match_name(name, priority)) {
		s.set_to_right(i + 1);
		continue;
	    } else if (!leading_star) {
		return false;
	    }
	}
	if (list == nil) {
	    list = new UniqueStringList;
	}
	UniqueString* u = new UniqueString(name);
	list->append(u);
	s.set_to_right(i + 1);
    }
    return true;
}

/*
 * Return the index of the next separator ("*" or ".") in the string.
 * If no separator is present, return -1.
 */

int StyleRep::find_separator(const String& s) {
    int n = s.length();
    for (int i = 0; i < n; i++) {
	char c = s[i];
	if (c == '*' || c == '.') {
	    return i;
	}
    }
    return -1;
}

/*
 * Check to see if a given name matches the style's name
 * or any of its prefixes.  Bump up the priority to override
 * less precise names.
 */

boolean StyleRep::match_name(const UniqueString& name, int& priority) {
    if (name_ != nil && name == *name_) {
	priority += 2;
	return true;
    }
    if (prefixes_ != nil) {
	for (ListItr(UniqueStringList) i(*prefixes_); i.more(); i.next()) {
	    if (name == *i.cur()) {
		priority += 1;
		return true;
	    }
	}
    }
    return false;
}

/*
 * Clear out any cached information about this style.
 */

void StyleRep::clear_info() {
    const Font* f;
    if (font_table_->find_and_remove(f, this)) {
	Resource::unref(f);
    }

    const Color* c;
    if (foreground_table_->find_and_remove(c, this)) {
	Resource::unref(c);
    }
    if (background_table_->find_and_remove(c, this)) {
	Resource::unref(c);
    }

    FancyColors* colors;
    if (fancy_table_->find_and_remove(colors, this)) {
	delete colors;
    }
}

/*
 * Note that a style has been modified.  Because of the way styles
 * are inherited, we must mark also all descendant styles as modified.
 */

void StyleRep::modify() {
    modified_ = true;
    if (children_ != nil) {
	for (ListItr(StyleList) i(*children_); i.more(); i.next()) {
	    i.cur()->rep_->modify();
	}
    }
}

/*
 * Update a style.  First we must ensure all modified ancestors are updated,
 * flush the cache of style information, and remove any cached attributes.
 */

void StyleRep::update() {
    if (!modified_) {
	return;
    }
    clear_info();
    if (parent_ != nil) {
	parent_->rep_->update();
	for (ListItr(AttributeList) i(*values_); i.more(); i.next()) {
	    StyleAttribute* a = i.cur();
	    if (a->is_cached()) {
		i.remove_cur();
		delete a;
	    }
	}
    }
    modified_ = false;
}

/*
 * Remove an attribute.  Because we might have multiple entries
 * with the same name, we must search the entire list.
 * Do nothing if the name is not found.
 */

void Style::remove_attribute(const String& name) {
    rep_->update();
    UniqueString s(name);
    String v;
    for (ListItr(AttributeList) i(*rep_->values_); i.more(); i.next()) {
	StyleAttribute* a = i.cur();
	if (a->match(s, v)) {
	    i.remove_cur();
	    delete a;
	    return;
	}
    }
}

/*
 * Return number of attributes.
 */

long Style::attributes() const {
    return rep_->values_->count();
}

/*
 * Return an attribute name and value for a given index.
 */

boolean Style::attribute(long i, String& name, String& value) const {
    AttributeList& list = *rep_->values_;
    if (i < 0 || i >= list.count()) {
	return false;
    }
    StyleAttribute& a = *list.item(i);
    name = a.name();
    value = a.value();
    return true;
}

/*
 * Triggers are not implemented yet.
 */

void Style::add_trigger(const String&, Action*) {
    /* not implemented */
}

/*
 * If the action parameter is nil, remove all triggers associated
 * with the name.
 */

void Style::remove_trigger(const String&, Action*) {
    /* not implemented */
}

/*
 * Find the value bound to a given name, if any.
 */

boolean Style::find_attribute(const String& name, String& value) const {
    StyleRep* s = rep_;
    s->update();
    if (s->find_normal(name, value)) {
	return true;
    }

    UniqueString uname(name);
    StyleList sl(20);
    Style* this_style = (Style*)this;
    sl.prepend(this_style);
    for (Style* style = s->parent_; style != nil; style = s->parent_) {
	s = style->rep_;
	if (s->wildcard_values_ != nil && s->find_wildcard(sl, uname, value)) {
	    return true;
	}
	if (s->find_normal(uname, value)) {
	    return true;
	}
	sl.prepend(style);
    }
    return false;
}

/*
 * Find normal attribute (as opposed to a wildcard), if defined.
 */

boolean StyleRep::find_normal(const UniqueString& name, String& value) {
    for (ListItr(AttributeList) a(*values_); a.more(); a.next()) {
	if (a.cur()->match(name, value)) {
	    return true;
	}
    }
    return false;
}

/*
 * Find best wildcard match, if any.
 */

boolean StyleRep::find_wildcard(
    StyleList& sl, const UniqueString& name, String& value
) {
    StyleWildcardMatchQuality q, best_match;
    String v;
    for (ListItr(WildcardList) w(*wildcard_values_); w.more(); w.next()) {
	w.cur()->wildcard_match(sl, name, q, v);
	if (q.defined() && (!best_match.defined() || q > best_match)) {
	    best_match = q;
	    value = v;
	}
    }
    return best_match.defined();
}

void StyleWildcard::wildcard_match(
    StyleList& sl, const UniqueString& s,
    StyleWildcardMatchQuality& q, String& v
) const {
    if (name() != s) {
	return;
    }
    q.zero();
    long cur_element = 0;
    long num_elements = path_->count();
    long cur_prefix = 0;
    long num_prefixes = sl.count();
    long try_prefix = 0;
    long possible_prefix_end = num_prefixes - num_elements + 1;
    while (cur_element >= 0 && cur_element < num_elements) {
	if (try_prefix >= possible_prefix_end) {
	    q.pop();
	    --cur_element;
	    --cur_prefix;
	    --possible_prefix_end;
	    try_prefix = cur_prefix + 1;
	} else {
	    UniqueString* element = path_->item(cur_element);
	    long index = StyleRep::prefix_index(sl.item(try_prefix), *element);
	    if (index != -1) {
		q.push(try_prefix, index);
		++cur_element;
		++cur_prefix;
		++possible_prefix_end;
		try_prefix = cur_prefix - 1;
	    }
	    ++try_prefix;
	}
    }
    if (q.defined()) {
	v = value();
    }
}

long StyleRep::prefix_index(Style* style, const UniqueString& prefix) {
    StyleRep* s = style->rep_;
    if (s->name_ != nil && *s->name_ == prefix) {
	return 0;
    }
    if (s->prefixes_ != nil) {
	long n = s->prefixes_->count();
	for (long i = 0; i < n; i++) {
	    UniqueString* p = s->prefixes_->item(i);
	    if (*p == prefix) {
		return i + 1;
	    }
	}
    }
    return -1;
}

/*
 * Short-hand
 */

boolean Style::find_attribute(const char* name, String& value) const {
    return find_attribute(String(name), value);
}

boolean Style::find_attribute(const String& name, long& value) const {
    String v;
    return find_attribute(name, v) && v.convert(value);
}

boolean Style::find_attribute(const char* name, long& value) const {
    return find_attribute(String(name), value);
}

boolean Style::find_attribute(const String& name, double& value) const {
    String v;
    return find_attribute(name, v) && v.convert(value);
}

boolean Style::find_attribute(const char* name, double& value) const {
    return find_attribute(String(name), value);
}

boolean Style::find_attribute(const String& name, Coord& value) const {
    String v;
    if (!find_attribute(name, v)) {
	return false;
    }
    String units(v);
    Coord pts = 1.0;
    const char* p = v.string();
    const char* end = p + v.length();
    boolean dot = false;
    for (; p < end; p++) {
	if (!dot && *p == '.') {
	    dot = true;
	} else if (!isspace(*p) && !isdigit(*p)) {
	    int i = p - v.string();
	    units.set_to_right(i);
	    if (units == "in") {
		pts = 72.0;
	    } else if (units == "em") {
		pts = font()->width('m');
	    } else if (units != "pt") {
		return false;
	    }
	    v.set_to_left(i);
	    break;
	}
    }
    if (v.convert(value)) {
	value *= pts;
	return true;
    }
    return false;
}

boolean Style::find_attribute(const char* name, Coord& value) const {
    return find_attribute(String(name), value);
}

boolean Style::value_is_on(const String& s) const {
    String v;
    if (!find_attribute(s, v)) {
	return false;
    }
    return v.case_insensitive_equal("on") || v.case_insensitive_equal("true");
}

boolean Style::value_is_on(const char* s) const {
    return value_is_on(String(s));
}

/*
 * Special typed style information for font, brush, colors, etc.
 */

const Font* Style::font() const {
    rep_->update();
    const Font* f;
    if (StyleRep::font_table_->find(f, rep_)) {
	return f;
    }
    String v;
    if (find_attribute(*StyleRep::font_string_, v) &&
	rep_->valid_font(v, rep_, f)
    ) {
	return f;
    }
    if (find_attribute(*StyleRep::Font_string_, v) &&
	rep_->valid_font(v, rep_, f)
    ) {
	return f;
    }
    return nil;
}

const Brush* Style::brush() const {
    /* unimplemented */
    rep_->update();
    return nil;
}

const Color* Style::foreground() const {
    return rep_->find_color(
	this, StyleRep::foreground_table_,
	StyleRep::foreground_string_, StyleRep::Foreground_string_
    );
}

const Color* Style::background() const {
    return rep_->find_color(
	this, StyleRep::background_table_,
	StyleRep::background_string_, StyleRep::Background_string_
    );
}

const Color* StyleRep::find_color(
    const Style* s, StyleToColor* table,
    const UniqueString* s1, const UniqueString* s2
) {
    update();
    const Color* c;
    if (table->find(c, this)) {
	return c;
    }
    String v;
    if (s->find_attribute(*s1, v) && valid_color(v, this, table, c)) {
	return c;
    }
    if (s->find_attribute(*s2, v) && valid_color(v, this, table, c)) {
	return c;
    }
    return nil;
}

const Color* Style::flat() const {
    rep_->update();
    FancyColors* colors;
    if (StyleRep::fancy_table_->find(colors, rep_)) {
	return colors->flat;
    }
    Display* d = Session::instance()->default_display();
    String v;
    if (find_attribute("flat", v)) {
	const Color* c = Color::lookup(d, v);
	if (c != nil) {
	    colors = new FancyColors;
	    colors->flat = c;
	    Resource::ref(colors->flat);
	    colors->light = c->brightness(0.6);
	    Resource::ref(colors->light);
	    colors->dull = c->brightness(-0.2);
	    Resource::ref(colors->dull);
	    colors->dark = c->brightness(-0.4);
	    Resource::ref(colors->dark);
	    StyleRep::fancy_table_->insert(rep_, colors);
	    return c;
	}
    }
    return nil;
}

const Color* Style::light() const {
    FancyColors* colors;
    if (flat() != nil && StyleRep::fancy_table_->find(colors, rep_)) {
	return colors->light;
    }
    return nil;
}

const Color* Style::dull() const {
    FancyColors* colors;
    if (flat() != nil && StyleRep::fancy_table_->find(colors, rep_)) {
	return colors->dull;
    }
    return nil;
}

const Color* Style::dark() const {
    FancyColors* colors;
    if (flat() != nil && StyleRep::fancy_table_->find(colors, rep_)) {
	return colors->dark;
    }
    return nil;
}

boolean StyleRep::valid_font(
    const String& name, StyleRep* requestor, const Font*& f
) {
    const Font* ff = Font::lookup(name);
    if (ff != nil) {
	Resource::ref(ff);
	font_table_->insert(this, ff);
	if (requestor != this) {
	    font_table_->insert(requestor, ff);
	}
	f = ff;
	return true;
    }
    return false;
}

boolean StyleRep::valid_color(
    const String& name, StyleRep* requestor,
    StyleToColor* table, const Color*& c
) {
    const Color* cc = Color::lookup(
	Session::instance()->default_display(), name
    );
    if (cc != nil) {
	Resource::ref(cc);
	table->insert(this, cc);
	if (requestor != this) {
	    table->insert(requestor, cc);
	}
	c = cc;
	return true;
    }
    return false;
}
