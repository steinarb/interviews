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
 * C++ interface to OS file system functions
 */

#ifndef os_fs_h
#define os_fs_h

#if defined(__cplusplus)
extern "C" {
#endif

extern int open(const char*, int, int=0644);
extern int creat(const char*, int);
extern int read(int, void*, unsigned);
extern int write(int, const void*, unsigned);
extern int close(int);
extern long lseek(int, long, int);
extern long tell(int);
extern int truncate(char* path, int length);
extern int ftruncate(int fd, int length);

extern int link(const char*, const char*);
extern int unlink(const char*);
extern int readlink(const char* path, char* buf, int bufsiz);

extern int dup(int);
extern int dup2(int, int);
extern int pipe(int*);

extern int ioctl(int, int ...);

extern int chmod(const char*, int);
extern int umask(int);
extern int chown(const char*, int, int);
extern int chroot(const char*);

extern int chdir(const char*);
extern char* getwd(char*);
extern char* getcwd(char*, int);
extern int mkdir(const char*, int);
extern int mknod(const char*, int, int);
extern int mount(const char*, const char*, int, int);
extern int umount(const char*);

extern int access(const char*, int);

extern void sync();

#if defined(__cplusplus)
}
#endif

#endif
