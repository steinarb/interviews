/*
 * glypheditor - a simple drawing editor-like application for tutorial purposes
 */

#include <InterViews/monoglyph.h>

class Button;
class GlyphViewer;
class TelltaleGroup;
class Window;

class GlyphEditor : public MonoGlyph {
public:
    GlyphEditor();
    Window* window();
    GlyphViewer* viewer();

    void new_session();
    void quit();

    virtual void allocate(Canvas*, const Allocation&, Extension&);
protected:
    Button* make_tool(TelltaleGroup*, const char*, unsigned int);
    Button* make_policy(TelltaleGroup*, const char*, unsigned int);
protected:
    GlyphViewer* _gviewer;
    Window* _w;
private:
    Glyph* interior();
    Glyph* buttons();
    Glyph* pacekeepers();
    Glyph* menus();
};

inline Window* GlyphEditor::window () { return _w; }
inline GlyphViewer* GlyphEditor::viewer () { return _gviewer; }
