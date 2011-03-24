/*
 * Copyright (c) 1987, 1988, 1989 Stanford University
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Stanford not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Stanford makes no representations about
 * the suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * STANFORD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL STANFORD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Property sheet of user-customizable variables.
 */

#ifndef propsheet_h
#define propsheet_h

#include <InterViews/defs.h>

typedef class StringId* PropertyName;
typedef const char* PropertyType;
typedef const char* PropertyValue;

class PropDir;
class PropPath;

/*
 * A property is defined by a name, optional type, and value.
 */

class PropertyDef {
public:
    PropertyName name;
    PropertyType type;
    PropertyValue value;

    PropertyDef () {}
    PropertyDef (PropertyName n) { name = n; value = nil; }
};

class PropertySheet {
public:
    PropertySheet();
    ~PropertySheet();

    /* individual property operations */
    boolean Get(PropertyDef&);
    boolean GetLocal(PropDir*, PropertyDef&);
    PropDir* MakeDir(const char* path);
    void Put(const char* path, const char* value, const char* type = nil);
    void PutLower(const char* path, const char* value, const char* type = nil);
    void PutLocal(
	PropDir*, const char* path, const char* value, const char* type = nil
    );
    void PutLocalLower(
	PropDir*, const char* path, const char* value, const char* type = nil
    );

    /* property tree navigation */
    PropDir* Find(PropertyName name);
    void Pop();
    void Push(PropDir*, boolean sibling);
    PropDir* Root();

    /* external interface */
    void LoadProperty(const char*);
    void LoadList(const char*);
    boolean LoadFile(const char* filename);
private:
    PropDir* cur;
    PropPath* head;
    PropPath* tail;

    void DoPut(
	PropDir* dir, const char* path, const char* value, const char* type,
	boolean override
    );
};

extern PropertySheet* properties;

inline void PropertySheet::Put (
    const char* path, const char* value, const char* type
) {
    DoPut(cur, path, value, type, true);
}

inline void PropertySheet::PutLower (
    const char* path, const char* value, const char* type
) {
    DoPut(cur, path, value, type, false);
}

inline void PropertySheet::PutLocal (
    PropDir* dir, const char* path, const char* value, const char* type
) {
    DoPut(dir, path, value, type, true);
}

inline void PropertySheet::PutLocalLower (
    PropDir* dir, const char* path, const char* value, const char* type
) {
    DoPut(dir, path, value, type, false);
}

#endif
