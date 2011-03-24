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
 * Root scene for a display.
 */

#ifndef world_h
#define world_h

#include <InterViews/scene.h>

/*
 * ParseGeometry return values contains one or more of these bits set.
 */

#define GeomNoValue 0x00
#define GeomXValue 0x01
#define GeomYValue 0x02
#define GeomWidthValue 0x04
#define GeomHeightValue 0x08
#define GeomAllValues 0x0F
#define GeomXNegative 0x10
#define GeomYNegative 0x20

class PropertyData {
public:
    const char* path;
    const char* value;
    const char* type;
};

enum OptionStyle {
    OptionPropertyNext,		/* Property and value are in argv[i+1]  */
    OptionValueNext,		/* argv[i+1] */
    OptionValueImplicit,	/* OptionDesc.value */
    OptionValueIsArg,		/* argv[i] */
    OptionValueAfter		/* &argv[i][strlen(OptionDesc.name)] */
};

class OptionDesc {
public:
    const char* name;
    const char* path;
    OptionStyle style;
    const char* value;
};

class World : public Scene {
public:
    World(const char* classname, int& argc, char* argv[]);
    World(const char* classname, OptionDesc*, int& argc, char* argv[]);
    World(
	const char* classname, PropertyData*, OptionDesc*,
	int& argc, char* argv[]
    );
    /* obsolete -- use one of above constructors */
    World(const char* instance = nil, const char* device = nil);
    ~World();

    void InsertPopup(Interactor*);
    void InsertPopup(
	Interactor*, Coord x, Coord y, Alignment = BottomLeft
    );

    void InsertTransient(Interactor*, Interactor*);
    void InsertTransient(
	Interactor*, Interactor*, Coord x, Coord y, Alignment = BottomLeft
    );

    void InsertToplevel(Interactor*, Interactor*);
    void InsertToplevel(
	Interactor*, Interactor*, Coord x, Coord y, Alignment = BottomLeft
    );

    void InsertApplication(Interactor*);
    void InsertApplication(
	Interactor*, Coord x, Coord y, Alignment = BottomLeft
    );

    void InsertIcon(Interactor*);
    void InsertIcon(
	Interactor*, Coord x, Coord y, Alignment = BottomLeft
    );

    int Width();
    int Height();
    int NPlanes();
    int NButtons();
    int PixelSize();
    Coord InvMapX(Coord);
    Coord InvMapY(Coord);

    /* obsolete -- use Interactor::GetAttribute */
    const char* GetDefault(const char*);
    const char* GetGlobalDefault(const char*);

    /* returns bitset of Geom... flags */
    unsigned int ParseGeometry(
	const char*, Coord&, Coord&, unsigned int&, unsigned int&
    );

    void SetHint(const char*);
    void RingBell(int);
    void SetKeyClick(int);
    void SetAutoRepeat(boolean);
    void SetFeedback(int thresh, int scale);

    void SetCurrent();
    void SetRoot(void*);
    void SetScreen(int);
    int Fileno();
    class WorldRep* Rep () { return rep; }
protected:
    void DoInsert(Interactor*, boolean, Coord& x, Coord& y);
    void DoChange(Interactor*);
    void DoRemove(Interactor*);
private:
    WorldRep* rep;

    void FinishInsert(Interactor*);
    unsigned int GetGeometry(
	Interactor*, Coord& x, Coord& y, unsigned int& w, unsigned int& h
    );
    void Init(const char* device);
    void FinishInit();
    void LoadUserDefaults();
    void ParseArgs(int&, char*[], OptionDesc[]);
    void Setup(const char*, int, char*[]);
    const char* UserDefaults();
    void SaveCommandLine(int, char*[]);
};

#endif
