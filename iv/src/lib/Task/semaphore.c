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

#include <Task/nil.h>
#include <Task/task.h>
#include <Task/sched.h>
#include <Task/semaphore.h>
#include <sys/time.h>

class Semaphore_queue {
private:
    friend class Semaphore;

    Task* entry;
    Semaphore_queue* next;
};

Semaphore::Semaphore(int value) {
    count = value;
    head = nil;
    tail = nil;
}

Semaphore::~Semaphore() {
    register Semaphore_queue* q, * qnext;
    for (q = head; q != nil; q = qnext) {
	qnext = q->next;
	delete q;
    }
}

void Semaphore::P() {
    Sched* s = Sched::instance();
    s->disable();
    if (count > 0) {
        count--;
	s->enable();
    } else {
        s->first()->change_state(Task::BLOCKED);
        if (head == nil) {
            head = new Semaphore_queue;
            tail = head;
            tail->entry = s->first();
            tail->next = nil;
        } else {
            tail->next = new Semaphore_queue;
            tail = tail->next;
            tail->next = nil;
            tail->entry = s->first();
        }
	s->enable();
        s->first()->yield();
    }
}

void Semaphore::V() {
    Sched* s = Sched::instance();
    s->disable();
    if (head == nil) {
        count++;
    } else {
        head->entry->change_state(Task::RUNNING);
        if (head == tail) {
            delete tail;
            head = nil;
            tail = nil;
        } else {
            Semaphore_queue* tmp = head;
            head = head->next;
            delete tmp;
        }
    }
    s->enable();
}
