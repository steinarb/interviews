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
#include "machine.h"
#include <Task/task.h>
#include <Task/sched.h>
#include <stdlib.h>
#include <stream.h>
#include <signal.h>
#include <sys/time.h>

Task::Task() {
    register Sched* s = Sched::instance();
    s->disable();
    pid = s->add_task(this);
    parent = s->first()->get_pid();
    return_value = 0;
    time.tv_sec = 0;
    time.tv_usec = 0;
    switches = 0;
    state = IDLE;
    s->enable();
}

/*
 * Special constructor for main task so that it doesn't
 * try to add itself to the scheduler queue.  The scheduler
 * creates the main task and uses it to initialize the queue.
 */

Task::Task(int main_pid) {
    pid = main_pid;
    parent = 0;
    state = RUNNING;
    sc_state = new sigcontext;
}

Task::~Task() {
    Sched::instance()->remove(this);
    if (state != EXITING && state != TERMINATED) {
        cout << "ERROR - Task desctructor called on active task";
	cout << ", pid = " << pid << "\n";
        ::exit(-1);
    }
}

void Task::start(unsigned int stacksize) {
    Sched::instance()->disable();
    sc_state = new sigcontext;
    stack_bottom = Machine::init_state(sc_state, stacksize);
    if (state == IDLE) {
	/* parent */
        state = RUNNING ;
	Sched::instance()->enable();
    } else {
	/* child */
        Task::exit(main());
    }
}

int Task::main() {
    ::exit(-1);
    return -1;
}

void Task::yield() {
    register Sched* s = Sched::instance();
    struct timeval time_remaining, time_used;
    time_remaining = reset_timer(s->get_time_slice()); 
    time_used = s->get_time_slice();
    time_used.tv_sec -= time_remaining.tv_sec; 
    time_used.tv_usec -= time_remaining.tv_usec;
    update_stats(time_used);
    Machine::save(sc_state);
    s->scheduler();
    Machine::restore(s->first()->get_sc_state());
    /* never gets here */
}

void Task::exit(int value) {
    register Sched* s = Sched::instance();
    s->disable();
    SchedIterator itr(s);
    for(Task* tmp = itr.first(); tmp != nil; tmp = itr.next()) {
        if (tmp->get_parent() == s->first()->get_pid()) {
	    /* if this task has children, they */
	    /* must be adopted by this' parent */
            tmp->change_parent(s->first()->get_parent());        
	}
        if (tmp->get_state() == WAITING) {
	    /* awake all waiting tasks         */
	    /* (they could be waiting on this) */
            tmp->change_state(RUNNING);	
	}
    }
    return_value = value;
    state = EXITING;
    s->enable();
    delete stack_bottom;
    delete sc_state;
    yield();
    /* never gets here */
}

void Task::wait() {
    register Sched* s = Sched::instance();
    Task* dead_task;
    SchedIterator i(s);
    for (;;) {
        for (dead_task = i.first(); dead_task != nil; dead_task = i.next()) {
            if (dead_task->state == EXITING && dead_task->pid == pid) {
		dead_task->change_state(TERMINATED);
		return;
	    }
        }
	s->first()->change_state(WAITING);
	s->first()->yield();
    }
}

struct sigcontext* Task::get_sc_state() { return sc_state; }
int Task::get_return() { return return_value; }
int Task::get_pid() { return pid; }
int Task::get_parent() { return parent; }
void Task::change_parent(int new_parent) { parent = new_parent; }
int Task::get_state() { return state; }

int Task::change_state(int new_state) {
    int old_state = state;
    state = new_state;
    return old_state;
}

void Task::sleep(int sleep_sec, int sleep_usec) {
    if (sleep_sec < 0 || sleep_usec < 0) {
	return;
    }
    wake_time = Sched::instance()->get_time();
    wake_time.tv_usec += sleep_usec;
    wake_time.tv_sec += sleep_sec + wake_time.tv_usec/1000000;
    wake_time.tv_usec %= 1000000;
    change_state(SLEEP);
    yield();
}

/*
 * Check for sleeping tasks due to awake; awakens task and returns true
 * if found.
 */

boolean Task::awaken() {
    if (state == SLEEP) {
        struct timeval now = Sched::instance()->get_time();
        if (wake_time.tv_sec == now.tv_sec && wake_time.tv_usec < now.tv_usec
             || wake_time.tv_sec < now.tv_sec) {
            change_state(RUNNING);
            return true;
        }
    }
    return false;
}

void Task::update_stats(struct timeval time_passed) {
    time.tv_usec += time_passed.tv_usec;
    time.tv_sec += time_passed.tv_sec + time.tv_usec/1000000;
    time.tv_usec %= 1000000;
    Sched::instance()->update_clock(time_passed);
    switches++;
}

struct timeval Task::time_elapsed() { return time; }
int Task::get_switches() { return switches; }

void Task::print_stats() {
    cout << "    Task pid = " << pid << ":";
    if (pid == 0) cout << " <- main task";
    if (pid == 1) cout << " <- idle task";
    cout << "\n             time elapsed = " << time.tv_sec << " secs, ";
    cout << time.tv_usec << " usecs\n";
    cout << "           total switches = " << switches << "\n";
    cout << "             return value = " << return_value << "\n";
}

static Task* find_exiting_task() {
    register Sched* s = Sched::instance();
    SchedIterator itr(s);
    for (Task* temp = itr.first(); temp != nil; temp = itr.next()) {
        if (temp->get_state() == Task::EXITING && 
             temp->get_parent() == s->first()->get_pid()
	) {
            return temp;
	}
    }
    return nil;
}

Task* Task::waitany() {
    register Sched* s = Sched::instance();
    Task* dead_task;
    while ((dead_task = find_exiting_task()) == nil) {
        s->first()->change_state(WAITING);
        s->first()->yield();
    }
    dead_task->change_state(TERMINATED);
    return dead_task;
}

MainTask::MainTask() : Task(0) { }

Idler::Idler() {
    pid = 1;
}

int Idler::main() {
    change_state(IDLE);
    yield();
    for (;;) {
        change_state(IDLE);
	sigpause(0);
	yield();
    }
}
