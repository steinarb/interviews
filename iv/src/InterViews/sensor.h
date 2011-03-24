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
 * Sensors describe input events of interest.
 */

#ifndef sensor_h
#define sensor_h

#include <InterViews/event.h>
#include <InterViews/resource.h>

class Sensor : public Resource {
public:
    Sensor();
    Sensor(Sensor*);
    ~Sensor();

    void Catch(EventType);
    void CatchButton(EventType, int);
    void CatchChannel(int);
    void CatchTimer(int, int);
    void Ignore(EventType);
    void IgnoreButton(EventType, int);
    void IgnoreChannel(int);
    void CatchRemote();
    void IgnoreRemote();
    virtual boolean Interesting(Event&);
protected:
    unsigned mask;
    unsigned down[8];
    unsigned up[8];
    static unsigned channels;
    static int maxchannel;
    boolean timer;
    int sec, usec;

    int ButtonIndex (unsigned b) { return (b >> 5) & 07; }
    int ButtonFlag (unsigned b) { return 1 << (b & 0x1f); }
    void SetButton (unsigned a[], unsigned b) {
	a[ButtonIndex(b)] |= ButtonFlag(b);
    }
    void ClearButton (unsigned a[], unsigned b) {
	a[ButtonIndex(b)] &= ~ButtonFlag(b);
    }
    boolean ButtonIsSet (unsigned a[], unsigned b) {
	return (a[ButtonIndex(b)] & ButtonFlag(b)) != 0;
    }
    void SetMouseButtons (unsigned a[]) { a[0] |= 0x7; }
    void ClearMouseButtons (unsigned a[]) { a[0] &= ~0x7; }
    boolean MouseButtons (unsigned a[]) { return (a[0] & 0x7) != 0; }
private:
    friend class Interactor;
    friend class Scene;
};

extern Sensor* allEvents;
extern Sensor* onoffEvents;
extern Sensor* updownEvents;
extern Sensor* noEvents;

#endif
