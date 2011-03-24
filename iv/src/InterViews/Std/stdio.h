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
 * C++ interface to C Standard I/O library
 */

#ifndef stdio_h

#if defined(__cplusplus)
extern "C" {
#endif

/* change the names to protect the type-checking */
#define fopen stdio_h_fopen
#define fdopen stdio_h_fdopen
#define freopen stdio_h_fdopen
#define fclose stdio_h_fclose
#define popen stdio_h_popen
#define pclose stdio_h_pclose
#define fread stdio_h_fread
#define fwrite stdio_h_fwrite
#define fseek stdio_h_fseek
#define ftell stdio_h_ftell
#define fflush stdio_h_fflush
#define rewind stdio_h_rewind
#define fgets stdio_h_fgets
#define gets stdio_h_gets
#define puts stdio_h_puts
#define fputs stdio_h_fputs
#define getw stdio_h_getw
#define fgetc stdio_h_fgetc
#define putw stdio_h_putw
#define fputc stdio_h_fputc
#define ungetc stdio_h_ungetc
#define printf stdio_h_printf
#define fprintf stdio_h_fprintf
#define sprintf stdio_h_sprintf
#define scanf stdio_h_scanf
#define fscanf stdio_h_fscanf
#define sscanf stdio_h_sscanf
#define setbuf stdio_h_setbuf
#define setbuffer stdio_h_setbuffer
#define setlinebuf stdio_h_setlinebuf
#define _filbuf stdio_h__filbuf
#define _flsbuf stdio_h__flsbuf
#define tmpfile stdio_h_tmpfile
#define ctermid stdio_h_ctermid
#define cuserid stdio_h_cuserid
#define tempnam stdio_h_tempnam
#define tmpnam stdio_h_tmpnam

/* use the standard C header file */
#include "//usr/include/stdio.h"

/* change the names back */
#undef fopen
#undef fdopen
#undef freopen
#undef fclose
#undef popen
#undef pclose
#undef fread
#undef fwrite
#undef fseek
#undef ftell
#undef fflush
#undef rewind
#undef fgets
#undef gets
#undef puts
#undef fputs
#undef getw
#undef fgetc
#undef putw
#undef fputc
#undef ungetc
#undef printf
#undef fprintf
#undef sprintf
#undef scanf
#undef fscanf
#undef sscanf
#undef setbuf
#undef setbuffer
#undef setlinebuf
#undef _filbuf
#undef _flsbuf
#undef tmpfile
#undef ctermid
#undef cuserid
#undef tempnam
#undef tmpnam

/* just in case standard header file didn't */
#ifndef stdio_h
#define stdio_h
#endif

extern FILE* fopen(const char* name, const char* type);
extern FILE* fdopen(int fd, const char* type);
extern FILE* freopen(const char* name, const char* type, FILE*);
extern int fclose(FILE*);
extern FILE* popen(const char* command, const char* type);
extern int pclose(FILE*);

extern int fread(char*, unsigned int, int, FILE*);
extern int fwrite(const char*, unsigned int, int, FILE*);
extern int fseek(FILE*, long, int);
extern long ftell(const FILE*);
extern int fflush(FILE*);
extern void rewind(FILE*);

extern char* fgets(char*, int, FILE*);
extern char* gets(char*);
extern int puts(const char*);
extern int fputs(const char*, FILE*);
extern int getw(FILE*);
extern int fgetc(FILE*);
extern int putw(int, FILE*);
extern int fputc(int, FILE*);
extern int ungetc(int, FILE*);

extern int printf(const char* ...);
extern int fprintf(FILE*, const char* ...);
extern int sprintf(char*, const char* ...);
extern int scanf(const char* ...);
extern int fscanf(FILE*, const char* ...);
extern int sscanf(char*, const char* ...);

#if !defined(clearerr)
extern void clearerr(FILE*);
#endif

extern void setbuf(FILE*, char*);

extern int _filbuf(FILE*);
extern int _flsbuf(unsigned, FILE*);

extern FILE* tmpfile();
extern char* ctermid(char*);
extern char* cuserid(char*);
extern char* tempnam(char*, char*);
extern char* tmpnam(char*);

#if defined(sgi)
/* Used in the getc/putc macros when shared processes are in use. */
extern int _semgetc(FILE*);
extern int _semputc(FILE*);
#endif

#if defined(__cplusplus)
}
#endif

#endif
