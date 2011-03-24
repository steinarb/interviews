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

#ifndef mt_task_h
#define mt_task_h

#include <Task/boolean.h>
#include <Task/nil.h>
#include <sys/signal.h>
#include <sys/time.h>

class Sched;

static const unsigned int Task_stacksize = 0x1000;

class Task {
public:
    enum State { RUNNING, IDLE, EXITING, TERMINATED, BLOCKED, WAITING, SLEEP };

    Task();
    virtual ~Task();

    virtual void start(unsigned int stacksize = Task_stacksize);
    virtual int main();

    void sleep(int sleep_sec, int sleep_usec = 0);
    void yield();
    void exit(int);
    void wait();

    struct sigcontext* get_sc_state();
    int get_pid();
    int get_parent();
    void change_parent(int);
    int get_state();
    int change_state(int);
    int get_return();
    boolean awaken();
    void update_stats(struct timeval);
    void print_stats();
    struct timeval time_elapsed();
    int get_switches();

    static Task* waitany();
protected:
    Task(int main_pid);
private:
    friend class MainTask;
    friend class Idler;

    int pid;
    int parent;
    struct sigcontext* sc_state;
    char* stack_bottom;
    int state;
    int return_value;
    struct timeval wake_time;
    struct timeval time;
    int switches;
};

class MainTask : public Task {
public:
    MainTask();
};

class Idler : public Task {
public:
    Idler();

    virtual int main();
};

#endif
