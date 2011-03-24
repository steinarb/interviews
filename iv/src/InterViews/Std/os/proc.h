/*
 * Copyright (c) 1989 Stanford University
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
 * C++ interface to OS process management functions
 */

#ifndef os_proc_h
#define os_proc_h

#if defined(__cplusplus)
extern "C" {
#endif

extern int system(const char*);
extern int fork();
extern int vfork();
extern int execl(const char*, const char* ...);
extern int execle(const char*, const char* ...);
extern int execlp(const char*, const char* ...);
extern int execv(const char*, const char**);
extern int execve(const char*, const char**, char**);
extern int execvp(const char*, const char**);
extern int wait(int*);
extern int kill(int, int);
extern void exit(int);
extern void _exit(int);
extern int nice(int);

extern int getpgrp();
extern int getpid();
extern int getppid();
extern int setpgrp(int pid, int pgrp);

#if defined(__cplusplus)
}
#endif

#endif
