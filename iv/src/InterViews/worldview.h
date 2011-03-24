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
 * Useful for writing window managers.
 */

#ifndef worldview_h
#define worldview_h

#include <InterViews/interactor.h>

class World;

typedef void* RemoteInteractor;

class WorldView : public Interactor {
public:
    WorldView(World*);
    ~WorldView();

    RemoteInteractor Find(Coord x, Coord y);
    RemoteInteractor Choose(Cursor*, boolean waitforup = true);
    virtual void InsertRemote(RemoteInteractor);
    virtual void ChangeRemote(
	RemoteInteractor, Coord left, Coord top, unsigned int w, unsigned int h
    );
    void Move(RemoteInteractor, Coord left, Coord top);
    void Change(
	RemoteInteractor, Coord left, Coord top, unsigned int w, unsigned int h
    );
    void Raise(RemoteInteractor);
    void Lower(RemoteInteractor);
    void Focus(RemoteInteractor);
    RemoteInteractor GetFocus () { return curfocus; }
    void GetList(RemoteInteractor*&, unsigned int&);
    void FreeList(RemoteInteractor*);
    void GetInfo(RemoteInteractor, Coord& x1, Coord& y1, Coord& x2, Coord& y2);
    boolean GetHints(RemoteInteractor, Coord& x, Coord& y, Shape&);
    void SetHints(RemoteInteractor, Coord x, Coord y, Shape&);
    RemoteInteractor GetIcon(RemoteInteractor);
    void AssignIcon(RemoteInteractor i, RemoteInteractor icon);
    void UnassignIcon(RemoteInteractor i);
    char* GetName(RemoteInteractor);
    RemoteInteractor TransientOwner(RemoteInteractor);
    void RedrawAll();
protected:
    class World* world;

    void GrabMouse(Cursor*);
    void UngrabMouse();
    boolean GrabButton(unsigned button, unsigned modifiers, Cursor*);
    void UngrabButton(unsigned button, unsigned modifiers);
    void Lock();
    void Unlock();
    void ClearInput();
    void MoveMouse(Coord x, Coord y);
    void Map(RemoteInteractor);
    void MapRaised(RemoteInteractor);
    void Unmap(RemoteInteractor);
private:
    RemoteInteractor curfocus;

    void Init(World*);
};

#endif
