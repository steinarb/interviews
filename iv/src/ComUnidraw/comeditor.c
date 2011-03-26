/*
 * Copyright (c) 1994-1996 Vectaport Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of the copyright holders not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  The copyright holders make
 * no representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 */

#include <ComUnidraw/grfunc.h>
#include <ComUnidraw/comeditor.h>
#include <ComUnidraw/comterp-iohandler.h>
#include <ComUnidraw/nfunc.h>
#include <ComUnidraw/plotfunc.h>

#include <ComTerp/ctrlfunc.h>
#include <ComTerp/comterpserv.h>
#include <ComTerp/strmfunc.h>

#include <OverlayUnidraw/ovcomps.h>

#include <Unidraw/catalog.h>
#include <Unidraw/keymap.h>
#include <Unidraw/kybd.h>
#include <Unidraw/unidraw.h>

#include <InterViews/frame.h>

#include <Attribute/attrlist.h>

/*****************************************************************************/

ComEditor::ComEditor(OverlayComp* comp, OverlayKit* kit) 
: OverlayEditor(false, kit) {
    Init(comp, "ComEditor");
}

ComEditor::ComEditor(const char* file, OverlayKit* kit)
: OverlayEditor(false, kit)
{
    if (file == nil) {
	Init();

    } else {
	Catalog* catalog = unidraw->GetCatalog();
	OverlayComp* comp;

	if (catalog->Retrieve(file, (Component*&) comp)) {
	    Init(comp);

	} else {
	    Init();
	    fprintf(stderr, "drawserv: couldn't open %s\n", file);
	}
    }
}

ComEditor::ComEditor(boolean initflag, OverlayKit* kit) 
: OverlayEditor(initflag, kit) {
  _terp = nil;
}

void ComEditor::Init (OverlayComp* comp, const char* name) {
    if (!comp) comp = new OverlayIdrawComp;
    _terp = new ComTerpServ();
    AddCommands(_terp);
    char buffer[BUFSIZ];
    sprintf(buffer, "Comdraw%d", ncomterp());
    add_comterp(buffer, _terp);
    _overlay_kit->Init(comp, name);
    
}

void ComEditor::InitCommands() {
    if (!_terp) 
      _terp = new ComTerpServ();
    const char* comdraw_off_str = unidraw->GetCatalog()->GetAttribute("comdraw_off");
    if ((!comterplist() || comterplist()->Number()==1) &&
	(comdraw_off_str ? strcmp(comdraw_off_str, "false")==0 : true))
      _terp_iohandler = new ComTerpIOHandler(_terp, stdin);
    else
      _terp_iohandler = nil;
#if 0
    _terp->add_defaults();
    AddCommands(_terp);
#endif
}

void ComEditor::AddCommands(ComTerp* comterp) {
    comterp->add_defaults();

    comterp->add_command("rect", new CreateRectFunc(comterp, this));
    comterp->add_command("line", new CreateLineFunc(comterp, this));
    comterp->add_command("ellipse", new CreateEllipseFunc(comterp, this));
    comterp->add_command("text", new CreateTextFunc(comterp, this));
    comterp->add_command("multiline", new CreateMultiLineFunc(comterp, this));
    comterp->add_command("openspline", new CreateOpenSplineFunc(comterp, this));
    comterp->add_command("polygon", new CreatePolygonFunc(comterp, this));
    comterp->add_command("closedspline", new CreateClosedSplineFunc(comterp, this));

    comterp->add_command("font", new FontFunc(comterp, this));
    comterp->add_command("brush", new BrushFunc(comterp, this));
    comterp->add_command("pattern", new PatternFunc(comterp, this));
    comterp->add_command("colors", new ColorFunc(comterp, this));

    comterp->add_command("nfonts", new NFontsFunc(comterp, this));
    comterp->add_command("nbrushes", new NBrushesFunc(comterp, this));
    comterp->add_command("npatterns", new NPatternsFunc(comterp, this));
    comterp->add_command("ncolors", new NColorsFunc(comterp, this));

    comterp->add_command("select", new SelectFunc(comterp, this));
    comterp->add_command("move", new MoveFunc(comterp, this));
    comterp->add_command("scale", new ScaleFunc(comterp, this));
    comterp->add_command("rotate", new RotateFunc(comterp, this));

    comterp->add_command("pan", new PanFunc(comterp, this));
    comterp->add_command("smallpanup", new PanUpSmallFunc(comterp, this));
    comterp->add_command("smallpandown", new PanDownSmallFunc(comterp, this));
    comterp->add_command("smallpanleft", new PanLeftSmallFunc(comterp, this));
    comterp->add_command("smallpanright", new PanRightSmallFunc(comterp, this));
    comterp->add_command("largepanup", new PanUpLargeFunc(comterp, this));
    comterp->add_command("largepandown", new PanDownLargeFunc(comterp, this));
    comterp->add_command("largepanleft", new PanLeftLargeFunc(comterp, this));
    comterp->add_command("largepanright", new PanRightLargeFunc(comterp, this));
    
    comterp->add_command("zoom", new ZoomFunc(comterp, this));
    comterp->add_command("zoomin", new ZoomInFunc(comterp, this));
    comterp->add_command("zoomout", new ZoomOutFunc(comterp, this));

    comterp->add_command("tilefile", new TileFileFunc(comterp, this));

    comterp->add_command("update", new UpdateFunc(comterp, this));
    comterp->add_command("ncols", new NColsFunc(comterp, this));
    comterp->add_command("nrows", new NRowsFunc(comterp, this));
    comterp->add_command("handles", new HandlesFunc(comterp, this));

    if (OverlayKit::bincheck("plotmtv"))
      comterp->add_command("barplot", new BarPlotFunc(comterp, this));
}



