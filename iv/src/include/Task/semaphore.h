/*
 * Copyright (c) 1990 Stanford University
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

#ifndef mt_semaphore_h
#define mt_semaphore_h

#include <Task/enter-scope.h>

class Semaphore_queue;

class Semaphore {
public:
    Semaphore(int);
    virtual ~Semaphore();

    virtual void P();
    virtual void V();

    void wait();
    void signal();
private:
    int count;
    Semaphore_queue* head;
    Semaphore_queue* tail;

    /*
     * To disallow direct assignment (i.e., s = 3;), we have to protect
     * an assignment operator.  Otherwise, the language allows the use
     * of the constructor and default assignment.
     */
    void operator =(int);

    /*
     * What does it mean to copy a semaphore?  For now, disallow.
     */
    void operator =(const Semaphore&);
};

inline void Semaphore::wait() { P(); }
inline void Semaphore::signal() { V(); }

#endif
