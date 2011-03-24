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

#ifndef mt_sched_h
#define mt_sched_h

#include <Task/boolean.h>
#include <sys/time.h>

static const unsigned long TIME_SLICE_DEFAULT = 500000 /* usecs */;
static const unsigned long MIN_TIME_SLICE = 5000 /* usecs */;

class Task;

class Taskq {
private:
    friend class Sched;
    friend class SchedIterator;

    Task* entry;
    Taskq* next;
};

class Sched {
public:
    virtual ~Sched();

    virtual Task* scheduler();
    virtual int add_task(Task*);
    virtual void submit(Task*);
    virtual void remove(Task*);
    virtual int get_qentries();
    virtual Task* first();

    boolean set_preemptable(boolean);
    struct timeval get_time();
    void update_clock(struct timeval);
    void set_time_slice(struct timeval);
    struct timeval get_time_slice();

    virtual void print_summary();
    virtual void debug();

    void disable();
    void enable();

    static void init();
    static void init(Sched*);
    static void init(Task*);
    static void init(Sched*, Task*);

    static Sched* instance();
    static Task* idle_task();
protected:
    Sched();
private:
    friend class SchedIterator;

    Taskq* head;
    Taskq* tail;
    int qentries;
    int last_pid;
    boolean preemptable;
    boolean save_preemptable;
    int signal_mask;
    int total_switches;
    struct timeval time_slice;
    struct timeval clock;

    static Sched* instance_;
    static Task* idle_task_;

    void initq();
    void start();
};

class SchedIterator {
public:
    SchedIterator(Sched*);

    Task* first();
    Task* last();
    Task* next();
private:
    Sched* this_sched;
    Taskq* this_task;
};

#endif
