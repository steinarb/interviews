/*
 * Copyright (c) 1991 Stanford University
 * Copyright (c) 1991 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and 
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Stanford and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Stanford and Silicon Graphics.
 * 
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 *
 * IN NO EVENT SHALL STANFORD OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
 * OF THIS SOFTWARE.
 */

#include <OS/directory.h>
#include <OS/memory.h>

/*
 * BSD tends to have things in <sys/dir.h>, System V uses <dirent.h>.
 * So far as I can tell, POSIX went with <dirent.h>.  Ultrix <dirent.h>
 * includes <sys/dir.h>, which is silly because <sys/dir.h>
 * needs <sys/types.h>.
 */
#include <sys/types.h>
#include <dirent.h>

#include <stdlib.h>
#include <string.h>

class DirectoryInfo {
private:
    friend class Directory;

    DirectoryInfo(DIR*);

    DIR* dir_;
    char** name_;
    int count_;
    int used_;
    boolean fill_;

    void fill();
};

DirectoryInfo::DirectoryInfo(DIR* d) {
    dir_ = d;
    name_ = nil;
    count_ = 0;
    used_ = 0;
    fill_ = true;
}

Directory::Directory(DirectoryInfo* d) {
    rep_ = d;
}

Directory::~Directory() {
    close();
    delete rep_;
}

DirectoryInfo* Directory::current() {
    return open(".");
}

DirectoryInfo* Directory::open(const char* name) {
    /* cast is to workaround bug in some opendir prototypes */
    DIR* d = opendir((char*)name);
    return (d == nil) ? nil : new DirectoryInfo(d);
}

int Directory::count() const {
    DirectoryInfo* d = rep_;
    d->fill();
    return d->used_;
}

const char* Directory::name(int i) const {
    DirectoryInfo* d = rep_;
    d->fill();
    if (i < 0 || i >= d->count_) {
	/* raise exception -- out of range */
	return nil;
    }
    return d->name_[i];
}

int Directory::index(const char* name) const {
    DirectoryInfo* d = rep_;
    d->fill();
    int i = 0, j = d->used_ - 1;
    while (i <= j) {
	int k = (i + j) / 2;
	int cmp = strcmp(name, d->name_[k]);
	if (cmp == 0) {
	    return k;
	}
	if (cmp > 0) {
	    i = k + 1;
	} else {
	    j = k - 1;
	}
    }
    return -1;
}

void Directory::close() {
    DirectoryInfo* d = rep_;
    if (d->dir_ != nil) {
	closedir(d->dir_);
	for (int i = 0; i < d->used_; i++) {
	    char* e = d->name_[i];
	    delete e;
	}
	delete d->name_;
	d->dir_ = nil;
    }
}

static int compare_entries(const void* k1, const void* k2) {
    char** s1 = (char**)k1;
    char** s2 = (char**)k2;
    return strcmp(*s1, *s2);
}

static unsigned int overflows = 0;

void DirectoryInfo::fill() {
    if (!fill_) {
	return;
    }
    fill_ = false;

    for (struct dirent* d = readdir(dir_); d != nil; d = readdir(dir_)) {
	if (used_ >= count_) {
	    ++overflows;
	    int new_count = count_ + 50*overflows;
	    char** new_name = new char*[new_count];
	    Memory::copy(name_, new_name, count_ * sizeof(char*));
	    delete name_;
	    name_ = new_name;
	    count_ = new_count;
	}
	name_[used_] = new char[strlen(d->d_name) + 1];
	strcpy(name_[used_], d->d_name);
	++used_;
    }
    qsort(name_, used_, sizeof(char*), &compare_entries);
}
