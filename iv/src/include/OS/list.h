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
 * Generic list implemented as dynamic array
 */

#ifndef os_list_h
#define os_list_h

#include <InterViews/boolean.h>
#include <generic.h>

#ifdef __GNUC__
#define delete_list_items() delete [size_] items_
#else
#define delete_list_items() delete [] items_
#endif

extern void ListImpl_range_error(long index);
extern long ListImpl_best_new_count(long count, unsigned size);

#define ListItr(List) name2(List,_Iterator)

#define declareList(List,T) \
class List { \
public: \
    List(long size = 0); \
    ~List(); \
\
    long count() const; \
    T& item(long index) const; \
    T* array(long index, long count); \
\
    void prepend(T& const); \
    void append(T& const); \
    void insert(long index, T& const); \
    void remove(long index); \
    void remove_all(); \
private: \
    T* items_; \
    long size_; \
    long count_; \
    long free_; \
}; \
\
inline long List::count() const { return count_; } \
\
inline T& List::item(long index) const { \
    if (index < 0 || index >= count_) { \
	ListImpl_range_error(index); \
    } \
    long i = index < free_ ? index : index + size_ - count_; \
    return items_[i]; \
} \
\
inline void List::append(T& const item) { insert(count_, item); } \
inline void List::prepend(T& const item) { insert(0, item); } \
\
class ListItr(List) { \
public: \
    ListItr(List)(List&); \
\
    boolean more() const; \
    T cur() const; \
    T& cur_ref() const; \
    void remove_cur(); \
    void next(); \
private: \
    List* list_; \
    long cur_; \
    long end_; \
}; \
\
inline boolean ListItr(List)::more() const { return cur_ < end_; } \
inline T ListItr(List)::cur() const { return list_->item(cur_); } \
inline T& ListItr(List)::cur_ref() const { \
    return list_->item(cur_); \
} \
inline void ListItr(List)::remove_cur() { list_->remove(cur_); } \
inline void ListItr(List)::next() { ++cur_; }

/*
 * List implementation
 */

#define implementList(List,T) \
List::List(long size) { \
    if (size > 0) { \
        size_ = ListImpl_best_new_count(size, sizeof(T)); \
        items_ = new T[size_]; \
    } else { \
        size_ = 0; \
        items_ = 0; \
    } \
    count_ = 0; \
    free_ = 0; \
} \
\
List::~List() { \
    delete_list_items(); \
} \
\
T* List::array(long index, long count) { \
    if (index + count <= free_) { \
        return items_ + index; \
    } else if (index >= free_) { \
        return items_ + index + size_ - count_; \
    } else { \
        long i; \
        for (i = 0; i < index + count - free_; ++i) { \
            items_[free_ + i] = items_[free_ + size_ - count_ + i]; \
        } \
        free_ = index + count; \
    } \
    return items_ + index; \
} \
\
void List::insert(long index, T& const item) { \
    if (count_ == size_) { \
        long size = ListImpl_best_new_count(size_ + 1, sizeof(T)); \
        T* items = new T[size]; \
        if (items_ != 0) { \
            register long i; \
            for (i = 0; i < free_; ++i) { \
                items[i] = items_[i]; \
            } \
            for (i = 0; i < count_ - free_; ++i) { \
                items[free_ + size - count_ + i] = \
                    items_[free_ + size_ - count_ + i]; \
            } \
            delete_list_items(); \
        } \
        items_ = items; \
        size_ = size; \
    } \
    if (index >= 0 && index <= count_) { \
	if (index < free_) { \
            for (register long i = free_ - index - 1; i >= 0; --i) { \
                items_[index + size_ - count_ + i] = items_[index + i]; \
            } \
        } else if (index > free_) { \
            for (register long i = 0; i < index - free_; ++i) { \
                items_[free_ + i] = items_[free_ + size_ - count_ + i]; \
            } \
        } \
        free_ = index + 1; \
        count_ += 1; \
        items_[index] = item; \
    } \
} \
\
void List::remove(long index) { \
    if (index >= 0 && index <= count_) { \
        if (index < free_) { \
            for (register long i = free_ - index - 2; i >= 0; --i) { \
                items_[size_ - count_ + index + 1 + i] = \
		    items_[index + 1 + i]; \
            } \
        } else if (index > free_) { \
            for (register long i = 0; i < index - free_; ++i) { \
                items_[free_ + i] = items_[free_ + size_ - count_ + i]; \
            } \
        } \
        free_ = index; \
        count_ -= 1; \
    } \
} \
\
void List::remove_all() { \
    count_ = 0; \
    free_ = 0; \
} \
\
ListItr(List)::ListItr(List)(List& list) { \
    list_ = &list; \
    cur_ = 0; \
    end_ = list.count(); \
}

#endif
