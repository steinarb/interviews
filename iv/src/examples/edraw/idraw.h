#ifndef idraw_h
#define idraw_h

#include <InterViews/enter-scope.h>
#include <OS/enter-scope.h>

class InputFile;
class Graphic;

class IdrawReader {
public:
    static Graphic* load(InputFile*);
    static Graphic* load(const char*);
};

#endif
