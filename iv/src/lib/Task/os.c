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
#include "state_save.h"
#include <Task/task.h>
#include <Task/sched.h>
#include <stream.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>

int sigvec(int, struct sigvec *, struct sigvec *);
void handler(int, int, struct sigcontext *);

void handler(int sig, int, struct sigcontext *scp) {
    Sched* sched = Sched::instance();
    Task* curtask = sched->first();
printf("signal %d in task %d\n", sig, curtask->get_pid());
    curtask->update_stats(sched->get_time_slice()); 
#ifdef mips
    struct sigcontext* tmp = curtask->get_sc_state();
    Machine::copy_state(tmp, scp);
    Task* newtask = sched->scheduler();
printf("new task is %d\n", newtask->get_pid());
    Machine::copy_state(scp, newtask->get_sc_state());
#endif
#ifdef sparc
    Machine::save(curtask->get_sc_state());
    Task* newtask = sched->scheduler();
    sched->set_preemptable(TRUE);
    Machine::restore(newtask->get_sc_state());
#endif
}

void init_signals(struct timeval time_slice) {
    struct itimerval value;
    struct sigvec vec;

    vec.sv_handler = (int (*)()) handler;
    vec.sv_mask = 0;
    vec.sv_flags = (SV_ONSTACK & SV_INTERRUPT);
//    sigvec(SIGVTALRM, &vec, (struct sigvec *) 0);
    sigvec(SIGALRM, &vec, (struct sigvec *) 0);

    value.it_interval.tv_sec = time_slice.tv_sec;
    value.it_interval.tv_usec = time_slice.tv_usec;
    value.it_value.tv_sec = time_slice.tv_sec;
    value.it_value.tv_usec = time_slice.tv_usec;

//    setitimer(ITIMER_VIRTUAL, &value, (struct itimerval *) 0);
    setitimer(ITIMER_REAL, &value, (struct itimerval *) 0);
}

struct timeval reset_timer(struct timeval time_slice) {
    struct itimerval value, time_used;
    value.it_interval.tv_sec = time_slice.tv_sec;
    value.it_interval.tv_usec = time_slice.tv_usec;
    value.it_value.tv_sec = time_slice.tv_sec;
    value.it_value.tv_usec = time_slice.tv_usec;
//    setitimer(ITIMER_VIRTUAL, &value, &time_used);
    setitimer(ITIMER_REAL, &value, &time_used);
    return time_used.it_value;
}

struct timeval signal_time() {
    struct itimerval value;
//    getitimer(ITIMER_VIRTUAL, &value);
    getitimer(ITIMER_REAL, &value);
    return(value.it_value);
}
