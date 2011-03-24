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
 * Input events.
 */

#ifndef event_h
#define event_h

#include <InterViews/defs.h>

enum EventType {
    MotionEvent,	/* mouse moved */
    DownEvent,		/* button pressed */
    UpEvent,		/* button released */
    KeyEvent,		/* key pressed, intepreted as ascii */
    EnterEvent,		/* mouse enters canvas */
    LeaveEvent,		/* mouse leaves canvas */
    ChannelEvent,	/* input pending on channel */
    TimerEvent,		/* time out on read */
    FocusInEvent,	/* focus for keyboard events */
    FocusOutEvent 	/* lose keyboard focus */
};

/* obsolete */
static const int OnEvent = EnterEvent;
static const int OffEvent = LeaveEvent;

/* mouse buttons */
static const int LEFTMOUSE = 0;
static const int MIDDLEMOUSE = 1;
static const int RIGHTMOUSE = 2;

typedef boolean EventFlag;

class EventRep;
class Interactor;
class World;

class Event {
public:
    Interactor* target;
    unsigned long timestamp;
    EventType eventType;
    Coord x, y;			/* mouse position relative to target */
    EventFlag control : 1;	/* true if down */
    EventFlag meta : 1;
    EventFlag shift : 1;
    EventFlag shiftlock : 1;
    EventFlag leftmouse : 1;
    EventFlag middlemouse : 1;
    EventFlag rightmouse : 1;
    unsigned char button;	/* button pressed or released, if any */
    unsigned short len;		/* length of ASCII string */
    char* keystring;		/* ASCII interpretation of event, if any */
    int channel;		/* set of channels ready */

    Event();
    ~Event();

    Event& operator=(Event&);
    void GetAbsolute(Coord&, Coord&);
    void GetAbsolute(World*&, Coord&, Coord&);
    EventRep* Rep() { return rep; }
private:
    friend class Sensor;
    friend class Interactor;

    World* w;			/* world in which event occurred */
    Coord wx, wy;		/* mouse position relative to world */
    char keydata[sizeof(int)];	/* keystring points here for simple mappings */
    EventRep* rep;

    void GetMotionInfo();
    void GetButtonInfo(EventType);
    void GetKeyInfo();
    void GetKeyState(unsigned);
    boolean GetCrossingInfo(EventType);
    void FindWorld(unsigned long);
};

#endif
