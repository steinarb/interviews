/*
 * Copyright (c) 1987, 1988, 1989 Stanford University
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
 * Interface to Persistent and related base classes.
 */

#ifndef persistent_h
#define persistent_h

#include <InterViews/defs.h>
#include <InterViews/Graphic/classes.h>

#define INVALIDUID	1
			  /* INVALIDUID & OBJMANUID must be consecutive uids */
#define OBJMANUID	3   

typedef unsigned UID;

class ListObject;
class ObjectMan;
class PFile;
class Persistent;

extern ObjectMan* TheManager;	    /* the well-known global object manager */

class Persistent {		    /* basic (persistent) Persistent */
    friend class Ref;
    friend class ObjectMan;
public:
    Persistent();
    virtual ~Persistent();

    virtual Persistent* GetCluster();
	/*
	 * returns head of cluster.  If GetCluster is redefined to return
	 * nil, then the object is assumed to be within the "current"
	 * cluster, i.e., it is a part of whatever cluster is currently
	 * being written out or read in.  If the object is Saved explicitly,
	 * a warning is issued and the object is NOT saved.
	 */
    virtual boolean Save();
    virtual boolean IsDirty();
    virtual void Touch();
    virtual void Clean();

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
protected:
    virtual UID getUID();

    void Warning(const char*);
    void Panic(const char*);
    void Fatal(const char*);

    virtual boolean write(PFile*);
    virtual boolean read(PFile*);
    virtual boolean writeObjects(PFile*);
    virtual boolean readObjects(PFile*);
    virtual boolean initialize();
};

#endif
