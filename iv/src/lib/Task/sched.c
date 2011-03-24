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

#include "os.h"
#include <Task/nil.h>
#include <Task/task.h>
#include <Task/sched.h>
#include <stdlib.h>
#include <stream.h>
#include <signal.h>
#include <sys/time.h>

Sched::Sched() {
    last_pid = 100;
    preemptable = false;
    time_slice.tv_usec = TIME_SLICE_DEFAULT;
    time_slice.tv_sec = 0;
    clock.tv_sec = 0;
    clock.tv_usec = 0;
    total_switches = 0;
}

Sched::~Sched() {
    set_preemptable(false);
    for (Taskq* tmp = head; tmp != nil; tmp = tmp->next) {
        delete tmp->entry;
        delete tmp;
    }
}

void Sched::start() {
    preemptable = true;
    init_signals(time_slice);
}

void Sched::initq() {
    Task* t = new MainTask;
    tail = head = new Taskq;
    tail->entry = t;
    tail->next = nil;
    qentries = 1;
}

int Sched::add_task(Task* new_task) {
    tail->next = new Taskq;
    tail = tail->next;
    tail->entry = new_task;
    tail->next = nil;
    last_pid++;
    qentries++;
    int new_pid = last_pid;
    return new_pid;
}

int Sched::get_qentries() { return qentries; }
Task* Sched::first() { return head->entry; }

void Sched::submit(Task* new_task) {
    Taskq* find_task;
    Taskq* prev_task = nil;
    new_task->change_state(Task::RUNNING);
    for (find_task = head; find_task != nil; find_task = find_task->next) {
        if (new_task == find_task->entry) {
            tail->next = head;
            head = find_task;
            if (prev_task != nil) {
		tail = prev_task;
	    }
            tail->next = nil;
            return;
        }
        prev_task = find_task;
    }
    cout << "ERROR - the scheduler submitted a task not in the run queue.\n";
    exit(-1);
}

void Sched::remove(Task* dead_task) {
    Taskq* find_task;
    Taskq* prev_task = nil;
    for (find_task = head; find_task != nil; find_task = find_task->next) {
        if (dead_task == find_task->entry) {
            if (dead_task == tail->entry) {
		tail = prev_task;
		tail->next = nil;
		delete(find_task);
		return;
            }
            prev_task->next = prev_task->next->next;
            delete find_task;
            return;
	}
	prev_task = find_task;
    }
}

void Sched::debug() {
    Taskq* tmp = head;
    cout << "Sched status: qentries = " << qentries << "\n";
    for (int i=0; i < qentries; i++) {
        cout << "     position #" << i+1 << " is pid ";
	cout << tmp->entry->get_pid() << ", state is ";
        switch(tmp->entry->get_state()) {
        case Task::RUNNING:
            cout << "RUNNING"; break;
        case Task::IDLE:
            cout << "IDLE"; break;
        case Task::EXITING:
            cout << "EXITING"; break;
        case Task::TERMINATED:
            cout << "TERMINATED"; break;
        case Task::BLOCKED:
            cout << "BLOCKED"; break;
        case Task::WAITING:
            cout << "WAITING"; break;
        case Task::SLEEP:
            cout << "SLEEP"; break;
        }
        if (tmp->entry->get_pid() == 0) {
	    cout << " <- main task";
	}
        if (tmp->entry->get_pid() == 1) {
	    cout << " <- idler task";
	}
        cout << endl;
        tmp = tmp->next;
    }
}

boolean Sched::set_preemptable(boolean b) {
    boolean old_preemptable = preemptable;
    if (preemptable != b) {
	preemptable = b;
	if (b) {
            sigsetmask(signal_mask);
	} else {
            signal_mask = sigblock(sigmask(SIGVTALRM));
        }
    }
    return old_preemptable;
}

void Sched::disable() { save_preemptable = set_preemptable(false); }
void Sched::enable() { set_preemptable(save_preemptable); }

void Sched::set_time_slice(struct timeval value) {
    if ((value.tv_sec * 1000000 + value.tv_usec) < MIN_TIME_SLICE ||
        value.tv_sec < 0 || value.tv_usec < 0
    ) {
	return;
    }
    time_slice.tv_usec = value.tv_usec;
    time_slice.tv_sec = value.tv_sec;
    init_signals(time_slice);
}

struct timeval Sched::get_time_slice() { return time_slice; }

struct timeval Sched::get_time() {
    struct timeval time_left = signal_time();
    struct timeval time;
    time.tv_usec = time_slice.tv_usec - time_left.tv_usec + clock.tv_usec;
    time.tv_sec = time_slice.tv_sec - time_left.tv_sec + clock.tv_sec
                  + time.tv_usec/1000000;
    time.tv_usec %= 1000000;
    return time;
}

void Sched::update_clock(struct timeval delta) {
    clock.tv_usec += delta.tv_usec;
    clock.tv_sec += delta.tv_sec + clock.tv_usec/1000000;
    clock.tv_usec %= 1000000;
    total_switches++;
}

void Sched::print_summary() {
    SchedIterator itr(this);
    Task* trial_task;
    cout << "Scheduler statistics summary:\n";
    for (trial_task=itr.first(); trial_task != nil; trial_task=itr.next()) {
        trial_task->print_stats();
    }
    cout << "Total Execution Time = " << clock.tv_sec;
    cout << " secs, " << clock.tv_usec << " usecs\n"; 
    cout << "Total context switches = " << total_switches << "\n";
}

/*
 * Default round-robin scheduler.
 */

Task* Sched::scheduler() {
    SchedIterator itr(this);
    Task* t = itr.first();
    for (t = itr.next(); t != nil; t = itr.next()) {
        if (t->get_state() == Task::RUNNING || t->awaken()) {
            submit(t);
            return t;
        }
    }
    t = itr.first();
    if (t->get_state() == Task::RUNNING || t->awaken()) {
        submit(t);
        return t;
    }
    t = idle_task();
    t->change_state(Task::RUNNING);
    submit(t);
    return t;
}

SchedIterator::SchedIterator(Sched* init_sched) {
    this_task = nil;
    this_sched = init_sched;
}

Task* SchedIterator::first() {
    this_task = this_sched->head;
    return this_task->entry;
}

Task* SchedIterator::last() {
    this_task = this_sched->tail;
    return this_task->entry;
}

Task* SchedIterator::next() {
    if (this_task->next == nil) {
	return nil;
    }
    this_task = this_task->next;
    return this_task->entry;
}

void Sched::init() {
    instance_ = new Sched;
    instance_->initq();
    idle_task_ = new Idler;
    idle_task_->start(0xf0);
    instance_->start();
}

void Sched::init(Sched* s) {
    instance_ = s;
    idle_task_ = new Idler;
    idle_task_->start(0xf0);
    instance_->start();
}

void Sched::init(Task* t) {
    instance_ = new Sched;
    instance_->initq();
    idle_task_ = t;
    instance_->start();
}

void Sched::init(Sched* s, Task* t) {
    instance_ = s;
    idle_task_ = t;
    instance_->start();
}

Sched* Sched::instance() { return instance_; }
Task* Sched::idle_task() { return idle_task_; }
