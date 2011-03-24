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

#include "machine.h"
#include "state_save.h"
#include <Task/task.h>
#include <Task/sched.h>
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>

#define COPY_SIZE 0x0c8 /* how far back parent's stack is copied */

char* Machine::init_state(struct sigcontext* sc_state, int sz) {
    sc_state->sc_mask = 0;
    char* new_stack = new char[sz];

    /* 28 = 7 words which are necessary to */
    /* ensure the stack copy aligns with the */
    /* sp which will be assigned in sig_save */
    copy_stack(
	(int*)((int)new_stack + sz), 
	(int*)(((int) &new_stack)-28)
    );

    /* copy current sigcontext into sc_state */
    /* child and parent both do this: */
    save(sc_state);

    float_save(sc_state);
    sc_state->sc_regs[29] = (int) new_stack + sz - COPY_SIZE;
    return new_stack;
}

void Machine::copy_state(struct sigcontext* dest, struct sigcontext* source) {
    *dest = *source;
}

void Machine::copy_stack(int* new_stack_top, int* stack_top) {
    int* target;
    for (int i = 0; i <= COPY_SIZE; i += 4) {
        target = (int*) ((int)new_stack_top - i);
        *target = *((int*) ((int)stack_top + COPY_SIZE - i));
    }
}

void Machine::save(struct sigcontext* my_state) {
    float_save(my_state);    
    sig_save(my_state);
}

void Machine::restore(struct sigcontext* next_state) {
     float_restore(next_state);    
     sig_restore(next_state);
}
