/*
 * Copyright (c) 1996-1998 Vectaport Inc., R.B. Kissh & Associates 
 * Copyright (c) 1994-1995 Vectaport Inc., Cartoactive Systems
 * Copyright (c) 1990, 1991 Stanford University
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

/*
 * OvImportCmd implementation.
 */

#include <OverlayUnidraw/grayraster.h>
#include <OverlayUnidraw/ovcatalog.h>
#include <OverlayUnidraw/ovclasses.h>
#include <OverlayUnidraw/ovfile.h>
#include <OverlayUnidraw/ovimport.h>
#include <OverlayUnidraw/ovraster.h>
#include <OverlayUnidraw/ovselection.h>
#include <OverlayUnidraw/ovstencil.h>
#include <OverlayUnidraw/oved.h>

#include <IVGlyph/gdialogs.h>
#include <IVGlyph/importchooser.h>

#include <Unidraw/Commands/align.h>
#include <Unidraw/Commands/edit.h>

#include <Unidraw/Components/grcomp.h>

#include <Unidraw/Graphic/rasterrect.h>
#include <Unidraw/Graphic/ustencil.h>

#include <Unidraw/clipboard.h>
#include <Unidraw/editor.h>
#include <Unidraw/statevars.h>
#include <Unidraw/unidraw.h>
#include <Unidraw/viewer.h>
#include <Unidraw/iterator.h>

#include <Attribute/paramlist.h>
#include <Attribute/attrvalue.h>

#include <InterViews/cursor.h>
#include <InterViews/dialog.h>
#include <InterViews/display.h>
#include <IV-look/dialogs.h>
#include <IV-look/kit.h>
#include <InterViews/bitmap.h>
#include <InterViews/raster.h>
#include <InterViews/session.h>
#include <InterViews/style.h>
#include <InterViews/tiff.h>
#include <InterViews/window.h>
#include <InterViews/transformer.h>

#include <TIFF/format.h>
#include <OS/string.h>

#include <pfstream.h>
#include <stdio.h>
#include <stream.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

/*****************************************************************************/

static int hexmap[] = {
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  0,  0,  0,  0,  0,  0,
     0, 10, 11, 12, 13, 14, 15,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0, 10, 11, 12, 13, 14, 15,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

const char PBM_MAGIC_BYTES[2] = {'P', '4'};
const char PGM_MAGIC_BYTES[2] = {'P', '5'};
const char PPM_MAGIC_BYTES[2] = {'P', '6'};
const char PBMA_MAGIC_BYTES[2] = {'P', '1'};
const char PGMA_MAGIC_BYTES[2] = {'P', '2'};
const char PPMA_MAGIC_BYTES[2] = {'P', '3'};

const char GZIP_MAGIC_BYTES[2] = {0x1f, 0x8b};
const char COMPRESS_MAGIC_BYTES[2] = {0x1f, 0x9d};

const char TIFF1_MAGIC_BYTES[2] = {0x4d, 0x4d}; /* TIFF with BIGENDIAN byte order */
const char TIFF2_MAGIC_BYTES[2] = {0x49, 0x49}; /* TIFF with LITLEENDIAN byte order */
const char SUN_MAGIC_BYTES[2] = {0x59, 0xa6}; /* Sun rasterfile (0x59a66a95) */
const char JPEG_MAGIC_BYTES[2] = {0xff, 0xd8}; 
/*****************************************************************************/


static void closef(FILE* file, boolean compressed) {
    if (compressed) {
        pclose(file); 
    }
    else { 
	fclose(file);
    }
}

class TileIterator {
public:
    TileIterator(int twidth, int theight, int width, int height);

    boolean Done();
    void Step(int& xbeg, int& xend, int& ybeg, int& yend);

private:
    boolean _done_row;
    boolean _done_column;
    int _twidth, _theight;
    int _width, _height;
    int _xcursor, _ycursor;
};

TileIterator::TileIterator(int twidth, int theight, int width, int height)
  : _twidth(twidth), _theight(theight), _width(width), _height(height),
    _done_row(false), _done_column(false), _xcursor(0), _ycursor(0)
{
}

boolean TileIterator::Done() {
    return _done_row && _done_column;
}

void TileIterator::Step(int& xbeg, int& xend, int& ybeg, int& yend) {
    _done_row = _done_column = false;

    xbeg = _xcursor;
    xend = _xcursor + _twidth - 1;
    ybeg = _ycursor;
    yend = _ycursor + _theight - 1;

    if (xend >= _width) {
        xend = _width - 1;
        _done_row = true;
    }

    if (yend >= _height) {
        yend = _height - 1;
        _done_column = true;
    }

    _xcursor = _done_row ? 0 : (_xcursor + _twidth);
    _ycursor = _done_row ? (_ycursor + _theight) : _ycursor;
}


/****************************************************************************/


static boolean CheckMagicBytes(const char* magic_bytes, const char* buffer) {
    return (buffer[0] == magic_bytes[0] && buffer[1] == magic_bytes[1]);
}

FILE* OvImportCmd::CheckCompression(
    FILE* file, const char *pathname, boolean& compressed
) {
    char cmd[256];
    
    if (!file || !fgets (cmd, 3, file)) {
        compressed = false;

    } else if (CheckMagicBytes(COMPRESS_MAGIC_BYTES, cmd)) {
        fclose (file);
        sprintf (cmd, "uncompress < %s", pathname);
        file = popen (cmd, "r");

        if (!file) {
            return NULL;
        }
        compressed = true;

    } else if (CheckMagicBytes(GZIP_MAGIC_BYTES, cmd)) {
        fclose (file);
        sprintf (cmd, "gunzip -c %s", pathname);
        file = popen (cmd, "r");

        if (!file) {
            return NULL;
        }
        compressed = true;

    } else {
        rewind (file);
        compressed = false;
    }

    return file;
}

const char* OvImportCmd::ReadCreator (const char* pathname) {
    FILE* file = fopen(pathname, "r");
    const int creator_size = 32;
    static char creator[creator_size];

    if (file != nil) {
        boolean compressed;
        char line[CHARBUFSIZE];
        
        file = CheckCompression(file, pathname, compressed);

        if (!file) {
            return NULL;
        }
	*creator = '\0';

        if (fgets(line, CHARBUFSIZE, file) != NULL) {

	    if (CheckMagicBytes(TIFF1_MAGIC_BYTES, line))
                strncpy(creator, "TIFF", creator_size);

	    else if (CheckMagicBytes(TIFF2_MAGIC_BYTES, line)) 
                strncpy(creator, "TIFF", creator_size);

	    else if (CheckMagicBytes(SUN_MAGIC_BYTES, line)) 
                strncpy(creator, "SUN", creator_size);

	    else if (CheckMagicBytes(PBM_MAGIC_BYTES, line)) 
                strncpy(creator, "PBM", creator_size);

	    else if (CheckMagicBytes(PGM_MAGIC_BYTES, line)) 
                strncpy(creator, "PGM", creator_size);

	    else if (CheckMagicBytes(PPM_MAGIC_BYTES, line)) 
                strncpy(creator, "PPM", creator_size);

	    else if (CheckMagicBytes(PBMA_MAGIC_BYTES, line)) 
                strncpy(creator, "PBM", creator_size);

	    else if (CheckMagicBytes(PGMA_MAGIC_BYTES, line)) 
                strncpy(creator, "PGM", creator_size);

	    else if (CheckMagicBytes(PPMA_MAGIC_BYTES, line)) 
                strncpy(creator, "PPM", creator_size);

	    else if (CheckMagicBytes(JPEG_MAGIC_BYTES, line)) 
	      strncpy(creator, "JPEG", creator_size);
    
            /* One-byte Magic numbers */
	    else {
		switch (line[0]) {
		case BM_MAGIC_NUM:
		    strncpy(creator, "BM", creator_size);
		    break;
		case ATK_MAGIC_NUM:
		    strncpy(creator, "ATK", creator_size);
		    break;
		case MP_MAGIC_NUM:
		    strncpy(creator, "MP", creator_size);
		    break;
		case X11_MAGIC_NUM:
		    strncpy(creator, "X11", creator_size);
		    break;
		case PCX_MAGIC_NUM:
		    strncpy(creator, "PCX", creator_size);
		    break;
		case IFF_MAGIC_NUM:
		    strncpy(creator, "IFF", creator_size);
		    break;
		case GIF_MAGIC_NUM:
		    strncpy(creator, "GIF", creator_size);
		    break;
		case RLE_MAGIC_NUM:
		    strncpy(creator, "RLE", creator_size);
		    break;
		}
	    }

        }   

	if (!*creator && line[0] == '%' && line[1] == '!' ) {
	    do {
		if (sscanf(line, "%%%%Creator: %s", creator)) {
		    break;
		    
		} else if (strcmp(line, "%%EndComments\n") == 0) {
		    break;
		}
	    } while (fgets(line, CHARBUFSIZE, file) != NULL);
	}

	if (!*creator) {
	    char *ptr = line;
	    while (isspace(*ptr)) ptr++;
	    int i = 0;
	    while (*ptr && !isspace(*ptr) && *ptr != '(' && i < creator_size-1) 
		creator[i++] = *ptr++;
	    creator[i] = '\0';
	}

        if (compressed) {
            pclose(file);

        } else {
            fclose(file);
        }
	if (compressed && strcmp(creator, "TIFF") == 0) {
	    cerr << "external compression not supported for TIFF format:  " << pathname << "\n";
	    return nil;
	}
	return creator;
    } else {
	cerr << "Unable to access image file:  " << pathname << "\n";
	return nil;
    }
}

const char* OvImportCmd::ReadCreator (istream& in, FileType& ftype) {
    const int creator_size = 32;
    static char creator[creator_size];

    boolean compressed;
    *creator = '\0';
    ftype = UnknownFile;

    const int linesz=80;
    char line[linesz];
    line[linesz-1] = '\0';
    int chcnt = 0;
    while(chcnt<linesz-1 && in.good() && in.get(line[chcnt]) && 
	  line[chcnt] && line[chcnt] != '\n') 
      chcnt++;

    if (CheckMagicBytes(COMPRESS_MAGIC_BYTES, line)) {
      ftype = CompressedFile;
      strncpy(creator, "COMPRESS", creator_size);
    }
    
    else if (CheckMagicBytes(GZIP_MAGIC_BYTES, line)) {
      ftype = CompressedFile;
      strncpy(creator, "GZIP", creator_size);
    }
    
    else if (CheckMagicBytes(TIFF1_MAGIC_BYTES, line))
      strncpy(creator, "TIFF", creator_size);
    
    else if (CheckMagicBytes(TIFF2_MAGIC_BYTES, line)) 
      strncpy(creator, "TIFF", creator_size);
    
    else if (CheckMagicBytes(SUN_MAGIC_BYTES, line)) 
      strncpy(creator, "SUN", creator_size);
    
    else if (CheckMagicBytes(PBM_MAGIC_BYTES, line)) 
      strncpy(creator, "PBM", creator_size);
    
    else if (CheckMagicBytes(PGM_MAGIC_BYTES, line)) 
      strncpy(creator, "PGM", creator_size);
    
    else if (CheckMagicBytes(PPM_MAGIC_BYTES, line)) 
      strncpy(creator, "PPM", creator_size);
    
    else if (CheckMagicBytes(PBMA_MAGIC_BYTES, line)) 
      strncpy(creator, "PBMA", creator_size);
    
    else if (CheckMagicBytes(PGMA_MAGIC_BYTES, line)) 
      strncpy(creator, "PGMA", creator_size);
    
    else if (CheckMagicBytes(PPMA_MAGIC_BYTES, line)) 
      strncpy(creator, "PPMA", creator_size);
    
    else if (CheckMagicBytes(JPEG_MAGIC_BYTES, line)) 
      strncpy(creator, "JPEG", creator_size);
    
    /* One-byte Magic numbers */
    else {
      switch (line[0]) {
      case BM_MAGIC_NUM:
	strncpy(creator, "BM", creator_size);
	break;
      case ATK_MAGIC_NUM:
	strncpy(creator, "ATK", creator_size);
	break;
      case MP_MAGIC_NUM:
	strncpy(creator, "MP", creator_size);
	break;
      case X11_MAGIC_NUM:
	strncpy(creator, "X11", creator_size);
	break;
      case PCX_MAGIC_NUM:
	strncpy(creator, "PCX", creator_size);
	break;
      case IFF_MAGIC_NUM:
	strncpy(creator, "IFF", creator_size);
	break;
      case GIF_MAGIC_NUM:
	strncpy(creator, "GIF", creator_size);
	break;
      case RLE_MAGIC_NUM:
	strncpy(creator, "RLE", creator_size);
	break;
      }
    }
    
    if (*creator && ftype==UnknownFile) ftype = OvImportCmd::RasterFile;
    

    if (!*creator && line[0] == '%' && line[1] == '!' ) {
      do {
	if (sscanf(line, "%%%%Creator: %s", creator)) {
	  break;
	  
	} else if (strcmp(line, "%%EndComments\n") == 0) {
	  break;
	}
      } while (in.getline(line, linesz) != NULL);
      chcnt = 0;
      if (*creator && ftype==UnknownFile) ftype = OvImportCmd::PostscriptFile;
    }
    
    
    if (!*creator) {
      char *ptr = line;
      while (isspace(*ptr)) ptr++;
      int i = 0;
      while (*ptr && !isspace(*ptr) && *ptr != '(' && i < creator_size-1) 
	creator[i++] = *ptr++;
      creator[i] = '\0';
      chcnt -= strlen(creator);
      if (*creator && ftype==UnknownFile) ftype = OvImportCmd::IvtoolsFile;
    }
    
    for (int i=chcnt; i>=0; i--) 
      in.unget();

    return creator;
}

static int gethex (FILE* file) {
    int c;
    while ((c = getc(file)) == ' ' || c == '\n') { }
    return (hexmap[c] << 4) + hexmap[getc(file)];
}

/*****************************************************************************/

ClassId OvImportCmd::GetClassId () { return OV_IMPORT_CMD; }

boolean OvImportCmd::IsA (ClassId id) {
    return OV_IMPORT_CMD == id || Command::IsA(id);
}

OvImportCmd::OvImportCmd (Editor* ed, ImportChooser* f) 
: Command(ed) { 
    Init(f);
}

OvImportCmd::OvImportCmd (ControlInfo* c, ImportChooser* f) 
: Command(c) { 
    Init(f);
}

void OvImportCmd::Init(ImportChooser* f) {
    chooser_ = f;    
    if (chooser_) 
	Resource::ref(chooser_);
    inptr_ = nil;
    path_ = nil;
    popen_ = false;
    preserve_selection_ = false;
    _is_url = false;
}

OvImportCmd::~OvImportCmd() {
  delete path_;
}

void OvImportCmd::instream(istream* in) { inptr_ = in; }

void OvImportCmd::pathname(const char* path, boolean popen) {
  path_ = path ? strdup(path) : nil; 
  popen_ = popen;
}

void OvImportCmd::preserve_selection(boolean ps) {
  preserve_selection_ = ps;
}

void OvImportCmd::Execute () { 

    GraphicComp* comp = nil;
    boolean from_dialog = !inptr_ && !path_;
    boolean empty = false;

    /* nothing known -- use dialog box */
    if (from_dialog)
      comp = PostDialog();

    /* pathname or command known */
    else if (path_) {
      filebuf fbuf;
      FILE* fptr = nil;

      /* open pathname or... */
      if (!popen_) {
	fbuf.open(path_, "r");
	inptr_ = new istream(&fbuf);

      /* popen command line */
      } else {
	fptr = popen(path_, "r");
	if (fptr) {
	  fbuf.attach(fileno(fptr));
	  inptr_ = new istream(&fbuf);
	}
      }

      boolean empty;
      if (inptr_) comp =  Import(*inptr_, empty);
      delete inptr_;
      inptr_ = nil;
      if (fptr) pclose(fptr);
    }
    
    /* input stream known */
    else 
      comp = Import(*inptr_, empty);
    
    if (comp != nil) {
        OverlaySelection* oldsel;
	if (preserve_selection_)
	  oldsel = new OverlaySelection((OverlaySelection*)GetEditor()->GetSelection());
	((OverlayEditor*)GetEditor())->DoAutoNewFrame();
	PasteCmd* paste_cmd = new PasteCmd(GetEditor(), new Clipboard(comp));
	paste_cmd->Execute();
	paste_cmd->Log();
	if (chooser_ && chooser_->centered()) {
	  GetEditor()->GetViewer()->Align(comp, /* Center */ 4);
	  if (GetEditor()->GetViewer()->GetGrid() != nil) {
	    GravityVar* grav = (GravityVar*) GetEditor()->GetState("GravityVar");
	    if (grav != nil && grav->IsActive()) {
	      // seems we need one for each dimension, x and y
	      AlignToGridCmd* algcmd = new AlignToGridCmd(GetEditor());
	      algcmd->Execute();
	      algcmd->Log();
	      AlignToGridCmd* alg2cmd = new AlignToGridCmd(GetEditor());
	      alg2cmd->Execute();
	      alg2cmd->Log();
	    }
	  }
	}


        // let components configures themselves with the editor
        ((OverlayEditor*)GetEditor())->InformComponents();
	if (preserve_selection_)
	  GetEditor()->SetSelection(oldsel);
        unidraw->Update();
    } else {
      if (!from_dialog && !empty) {
	Window* w = GetEditor()->GetWindow();
	w->cursor(defaultCursor);
	GAcknowledgeDialog::post(w, "import failed", nil, "import failed");
      }
    }
}

boolean OvImportCmd::Reversible () { return false; }

::Command* OvImportCmd::Copy () {
    OvImportCmd* copy = new OvImportCmd(CopyControlInfo());
    InitCopy(copy);
    copy->preserve_selection(preserve_selection_);
    return copy;
}

GraphicComp* OvImportCmd::PostDialog () {
    boolean imported = false;
    GraphicComp* comp = nil;
    Editor* ed = GetEditor();

    Style* style;
    boolean reset_caption = false;
    if (chooser_ == nil) {
        chooser_ = &ImportChooser::instance();
	Resource::ref(chooser_);
    } 
    style = chooser_->style();
    boolean again;
    while (again = chooser_->post_for(ed->GetWindow())) {
	const String* str = chooser_->selected();
	if (str != nil) {
	    NullTerminatedString ns(*str);
	    OverlayComp* idraw_comp = (OverlayComp*)ed->GetViewer()->GetGraphicView()->GetGraphicComp();
	    if (!idraw_comp->GetPathName() || strcmp(idraw_comp->GetPathName(), ns.string()) != 0 ) {
		OverlayCatalog* catalog = (OverlayCatalog*)unidraw->GetCatalog();
		catalog->SetParent(idraw_comp);
		ed->GetWindow()->cursor(hourglass);
		chooser_->twindow()->cursor(hourglass);
		style->attribute("caption", "                                   ");
		chooser_->twindow()->repair();
		chooser_->twindow()->display()->sync();
		comp = Import(ns.string());
		catalog->SetParent(nil);
		if (comp != nil) {
		    break;
		}
		style->attribute("caption", "Import failed                      ");
		reset_caption = true;
		ed->GetWindow()->cursor(arrow);
		chooser_->twindow()->cursor(arrow);
	    } else {
		style->attribute("caption", "Import failed (pathname recursion!)");
		reset_caption = true;
		ed->GetWindow()->cursor(arrow);
		chooser_->twindow()->cursor(arrow);
	    }
	}
    }
    chooser_->unmap();
    if (reset_caption) {
	style->attribute("caption", "");
    }
    if (!again)
	ed->GetWindow()->cursor(arrow);
    return comp;
}

GraphicComp* OvImportCmd::Import (const char* path) {
#if 0
    GraphicComp* comp = nil;
    boolean raster_import = false;

    /* pipe import from command */
    if (chooser_->from_command() || chooser_->auto_convert()) {
      FILE* fptr = nil;
      if (chooser_->auto_convert()) {
	char buffer[BUFSIZ];
	sprintf( buffer, "anytopnm %s", path );
	fptr = popen(buffer, "r");
      } else
        fptr = popen(path, "r");
      if (fptr) {
	filebuf fbuf;
	fbuf.attach(fileno(fptr));
	istream in(&fbuf);
	comp = Import(in);
      }
      pclose(fptr);
      return comp;
    }

    const char* creator = ReadCreator(path);
    OverlayCatalog* catalog = (OverlayCatalog*)unidraw->GetCatalog();

    if (creator && 
	(strcmp(creator, "drawtool") == 0 || strcmp(creator, "ov-idraw") == 0)
	&& catalog->Retrieve(path, (Component*&) comp)) {
      
      catalog->Forget(comp);
      if (chooser_->by_pathname()) {
	OverlayFileComp* ovfile = new OverlayFileComp();
	ovfile->SetPathName(path);
	ovfile->Append(comp);
	comp = ovfile;
      }
      
    } else if (creator && strcmp(creator, "X11") == 0) {
	comp = XBitmap_Image(path);
	raster_import = true;
    
    } else if (creator && strcmp(creator, "TIFF") == 0) {
	comp = TIFF_Image(path);
	raster_import = true;
    
    } else if (creator && strcmp(creator, "PBM") == 0) {
	comp = PBM_Image(path);
	raster_import = true;

    } else if (creator && strcmp(creator, "PGM") == 0) {
	comp = PGM_Image(path);
	raster_import = true;
    
    } else if (creator && strcmp(creator, "PPM") == 0) {
	comp = PPM_Image(path);
	raster_import = true;

    } else {
      
      if (catalog->Valid(path, (Component*&) comp)) 
	comp = (GraphicComp*) comp->Copy();
      else if (!catalog->IdrawCatalog::Retrieve(path, (Component*&) comp)) {
	catalog->Forget(comp);
	delete comp;
	comp = nil;
      }
    }

    if (comp && raster_import)
      ((RasterOvComp*)comp)->SetByPathnameFlag(chooser_->by_pathname());

    return comp;
#else

    GraphicComp* comp = nil;
    FILE* fptr = nil;
    boolean urlflag = false;
    if (chooser_ && chooser_->auto_convert()) {
      char buffer[BUFSIZ];
      sprintf( buffer, "anytopnm %s", path );
      fptr = popen(buffer, "r");
    } else if (chooser_ && chooser_->from_command()) {
      fptr = popen(path, "r");
    } else {
      if (ParamList::urltest(path)) {
	urlflag = true;
	char buffer[BUFSIZ];
	sprintf(buffer, "ivdl %s -", path);
	cerr << buffer << "\n";
	fptr = popen(buffer, "r");
      } else 
	fptr = fopen(path, "r");
      pathname(path);
      is_url(urlflag);
    }
    if (fptr) {
      filebuf fbuf;
      fbuf.attach(fileno(fptr));
      istream in(&fbuf);
      comp = Import(in);

      if (urlflag || 
	  (chooser_ && (chooser_->auto_convert() || chooser_->from_command())))
	pclose(fptr);
      else
	fclose(fptr);
    }    

    if (comp && pathname()) {
      ((OverlayComp*)comp)->SetPathName(pathname());
      if (chooser_) {
	((OverlayComp*)comp)->SetByPathnameFlag(chooser_->by_pathname());
	if (chooser_->by_pathname() && comp->IsA(OVERLAY_IDRAW_COMP)) {
	  OverlayFileComp* ovfile = new OverlayFileComp();
	  ovfile->SetPathName(path);
	  ovfile->Append(comp);
	  comp = ovfile;
	}
      }
    }
    pathname(nil);
    is_url(false);

    return comp;
#endif
}

GraphicComp* OvImportCmd::Import (istream& in) {
  boolean empty;
  return Import(in, empty);
}

GraphicComp* OvImportCmd::Import (istream& instrm, boolean& empty) {
    GraphicComp* comp = nil;
    Editor* ed = GetEditor();
    OverlayCatalog* catalog = (OverlayCatalog*)unidraw->GetCatalog();

    int len = 255;
    char buf[len];

    char ch;
    while (isspace(ch = instrm.get())); instrm.putback(ch);
    OvImportCmd::FileType filetype;
    const char* creator = ReadCreator(instrm, filetype);

    /* block of code to handle compressed files */
    istream* in;
    istream* gunzip_in = nil;
    filebuf* gunzip_fbuf = nil;
    FILE* gunzip_fptr = nil;
    boolean compressed = false;
    if (filetype==CompressedFile) {
      compressed = true;
      if (pathname() && !is_url()) {
	char buffer[BUFSIZ];
	sprintf(buffer, "gunzip -c %s", pathname());
	gunzip_fptr = popen(buffer, "r");
	if (gunzip_fptr) {
	  gunzip_fbuf = new filebuf();
	  gunzip_fbuf->attach(fileno(gunzip_fptr));
	  gunzip_in = new iostream(gunzip_fbuf);
	  in = gunzip_in;
	}
      } else {
	int newfd = Pipe_Filter(instrm, "gunzip -c");
	if (newfd != -1) {
	  gunzip_fbuf = new filebuf();
	  gunzip_fbuf->attach(newfd);
	  gunzip_in = new iostream(gunzip_fbuf);
	  in = gunzip_in;
	}
      }
      creator = ReadCreator(*in, filetype);
    } else
      in = &instrm;

    if (strcmp(creator, "drawtool") == 0 || strcmp(creator, "ov-idraw") == 0) { 
        Viewer* viewer = ed ? ed->GetViewer() : nil;
        OverlayComp* parent_comp =  viewer 
	  ? (OverlayComp*)viewer->GetGraphicView()->GetGraphicComp() 
	  : nil;
        comp = new OverlayIdrawComp(*in, pathname(), parent_comp);

    } else if (filetype == OvImportCmd::PostscriptFile) { 

      /* idraw formatted PostScript */
      if (strncmp(creator, "idraw", 5) == 0) 
	comp = catalog->ReadPostScript(*in);

      /* any other format PostScript */
      else {
	if (OverlayKit::bincheck("pstoedit")) {
	  FILE* pptr = nil;
	  int new_fd;
	  if (pathname() && !is_url()) {
	    char buffer[BUFSIZ];
	    if (compressed) 
	      sprintf(buffer, "gzip -c %s | pstoedit -f idraw", pathname());
	    else
	      sprintf(buffer, "pstoedit -f idraw %s", pathname());
	    pptr = popen(buffer, "r");
	    cerr << "input opened with " << buffer << "\n";
	    if (pptr) 
	      new_fd = fileno(pptr);
	  } else 
	    new_fd = Pipe_Filter(*in, "pstoedit -f idraw");
	  filebuf fbuf;
	  fbuf.attach(new_fd);
	  istream new_in(&fbuf);
	  comp = catalog->ReadPostScript(new_in);
	  if (pptr) pclose(pptr);
	} else
	  cerr << "pstoedit not found\n";
      }
      
    } else if (strncmp(creator, "PBM", 3)==0 ||
	       strncmp(creator, "PGM", 3)==0 ||
	       strncmp(creator, "PPM", 3)==0) {
      comp = PNM_Image(*in, creator);

    } else if (strncmp(creator, "GIF", 3)==0) {
      if (OverlayKit::bincheck("giftopnm")) {
	if (pathname() && !is_url()) {
	  char buffer[BUFSIZ];
	  if (compressed)
	    sprintf(buffer, "gzip -c %s | giftopnm", pathname());
	  else
	    sprintf(buffer, "giftopnm %s", pathname());
	  FILE* pptr = popen(buffer, "r");
	  if (pptr) {
	    cerr << "input opened with " << buffer << "\n";
	    filebuf fbuf;
	    fbuf.attach(fileno(pptr));
	    istream new_in(&fbuf);
	    comp = PNM_Image(new_in);
	    pclose(pptr);
	  }
	} else	
	  comp = PNM_Image_Filter(*in, "giftopnm");
      } else 
	cerr << "giftopnm not found\n";

    } else if (strncmp(creator, "TIFF", 4)==0) {
      if (pathname() && !is_url() && strcmp(pathname(),"-")!=0 && !compressed) 
	comp = TIFF_Image(pathname());
      else {
	if (OverlayKit::bincheck("tiftopnm"))
	  comp = PNM_Image_Filter(*in, "tiftopnm");
	else
	  cerr << "tiftopnm not found\n";
      }

    } else if (strncmp(creator, "JPEG", 4)==0) {
      if (OverlayKit::bincheck("stdcmapppm") && 
	  OverlayKit::bincheck("djpeg")) {
	if (pathname() && !is_url()) {
	  char buffer[BUFSIZ];
	  if (compressed) 
	    sprintf(buffer, "cm=`tmpnam`;stdcmapppm>$cm;gzip -c %s | djpeg -map $cm -dither fs -pnm;rm $cm", pathname());
	  else
	    sprintf(buffer, "cm=`tmpnam`;stdcmapppm>$cm;djpeg -map $cm -dither fs -pnm %s;rm $cm", pathname());
	  FILE* pptr = popen(buffer, "r");
	  if (pptr) {
	    cerr << "input opened with " << buffer << "\n";
	    filebuf fbuf;
	    fbuf.attach(fileno(pptr));
	    istream new_in(&fbuf);
	    comp = PNM_Image(new_in);
	    pclose(pptr);
	  }
	} else
	  comp = PNM_Image_Filter(*in, "cm=`tmpnam`;stdcmapppm>$cm;djpeg -map $cm -dither fs -pnm;rm $cm");
      } else
	cerr << "djpeg or stdcmapppm not found\n";

    }
    
    if (comp && comp->IsA(OVRASTER_COMP)) 
      ((RasterOvComp*)comp)->SetByPathnameFlag(false);
    
    if (comp)
      empty = false;
    else {
      empty = true;
      const char* ptr = creator;
      while( *ptr && empty) empty = isspace(*ptr++);
    }

    delete gunzip_in;
    delete gunzip_fbuf;
    if (gunzip_fptr)
      pclose(gunzip_fptr);

    return comp;
}

GraphicComp* OvImportCmd::TIFF_Image (const char* pathname) {
    GraphicComp* comp = nil;
    OverlayRaster* raster = TIFF_Raster(pathname);

    if (raster != nil) {
        comp = new RasterOvComp(new OverlayRasterRect(raster), pathname);
    }

    return comp;
}

OverlayRaster* OvImportCmd::TIFF_Raster (const char* pathname) {
    Raster* raster = TIFFRaster::load(pathname);
    OverlayRaster* ovraster = new OverlayRaster(*raster);
    delete raster;
    ovraster->flush();
    return ovraster;
}

boolean OvImportCmd::Tiling(int& width, int& height) {
    Catalog* catalog = unidraw->GetCatalog();

    const char* tile = catalog->GetAttribute("tile");
    if (tile && (!strcmp(tile, "true") || !strcmp(tile, "TRUE"))) {
        const char* cwidth = catalog->GetAttribute("twidth"); 
        const char* cheight = catalog->GetAttribute("theight");

        width = atoi(cwidth);
        height = atoi(cheight);

        if ((width <= 10) || (height <= 10)) {
            cerr << "tile dimensions must be greater than 10: no tiling"
                << " performed\n";
            return false;
        } else {
            return true;
        }
    } else {
        return false;
    }
}

// --------------------------------------------------------------------------

PPM_Helper::PPM_Helper(boolean is_ascii) : PortableImageHelper(is_ascii) {
}

boolean PPM_Helper::ppm() {
    return true;
}

int PPM_Helper::bytes_per_pixel() {
    return 3;
}

void PPM_Helper::read_write_pixel( FILE* infile, FILE* outfile ) {
    int red, green, blue;
    if (is_ascii()) fscanf(infile, "%d", &red); else red = getc(infile);
    putc(red, outfile);
    if (is_ascii()) fscanf(infile, "%d", &green); else green = getc(infile);
    putc(green, outfile);
    if (is_ascii()) fscanf(infile, "%d", &blue); else blue = getc(infile);
    putc(blue, outfile);
}

const char* PPM_Helper::magic() {
    return is_ascii() ? "P3" : "P6\n";
}

void PPM_Helper::read_poke(
    OverlayRaster* raster, FILE* file, u_long x, u_long y
) {
    int red, green, blue;
    if (is_ascii()) { 
      fscanf(file, "%d", &red); 
      fscanf(file, "%d", &green); 
      fscanf(file, "%d", &blue);  
      raster->poke( x, y,
		   float(red)/0xffff, float(green)/0xffff, float(blue)/0xffff, 
		   1.0 );
    } else {
      red = getc(file);
      green = getc(file);
      blue = getc(file);
      raster->poke(
		   x, y,
		   float(red)/0xff, float(green)/0xff, float(blue)/0xff, 
		   1.0 );
    }
}

OverlayRaster* PPM_Helper::create_raster( u_long w, u_long h ) {
    return new OverlayRaster(w, h);
}


// --------------------------------------------------------------------------

PGM_Helper::PGM_Helper(boolean is_ascii) : PortableImageHelper(is_ascii) {
}

boolean PGM_Helper::ppm() {
    return false;
}

int PGM_Helper::bytes_per_pixel() {
    return 1;
}

void PGM_Helper::read_write_pixel( FILE* infile, FILE* outfile ) {
    int gray;
    if (is_ascii())
        fscanf(infile, "%d", &gray);
    else
        gray = getc(infile);
    putc(gray, outfile);
}

const char* PGM_Helper::magic() {
    return is_ascii() ? "P2" : "P5\n";
}

void PGM_Helper::read_poke(
    OverlayRaster* raster, FILE* file, u_long x, u_long y
) {
    unsigned int gray;
    if (is_ascii()) {
        fscanf(file, "%d", &gray);
#if 0
	if (maxval()==65535) 
	  gray = gray >> 8;
#endif
    } else 
        gray = getc(file);
    raster->graypoke( x, y, gray );
}

OverlayRaster* PGM_Helper::create_raster( u_long w, u_long h ) {
  OverlayRaster* raster;
  if (RasterOvComp::UseGrayRaster()) {
    if (maxval() <= 255) 
      raster = new GrayRaster(w, h);
    else if (maxval() <= 655335) 
      raster = new GrayRaster(w, h, AttributeValue::UShortType);
    else
      raster = new GrayRaster(w, h, AttributeValue::FloatType);
  }
  else {
    raster = new OverlayRaster(w, h);
    raster->gray_flag(true);
  }
  return raster;
}


// --------------------------------------------------------------------------
OverlayRaster* OvImportCmd::PGM_Raster (
    const char* pathname, boolean delayed, OverlayRaster* raster,
    IntCoord xbeg, IntCoord xend, IntCoord ybeg, IntCoord yend
) {
    int width, height;
    boolean compressed;
    boolean tiled;
    int ignore;
    PortableImageHelper* pih;

    FILE* file = Portable_Raster_Open(
        pih, pathname, 0, width, height, compressed, tiled, ignore, ignore
    );

    if (file) {
        raster = PI_Raster_Read(
            pih, file, width, height, compressed, tiled, delayed, raster, 
            xbeg, xend, ybeg, yend
        );
    }
    else {
        raster = nil;
    }

    return raster;
}


GraphicComp* OvImportCmd::PGM_Image(const char* pathname) {
    GraphicComp* comp = nil;
    int width, height;
    boolean compressed;
    boolean tiled;
    int twidth, theight;
    PortableImageHelper* pih;

    FILE* file = Portable_Raster_Open(
        pih, pathname, 0, width, height, compressed, tiled, twidth, theight
    );
    if (file) {
        comp = Create_Comp(
            pih, file, pathname, width, height, compressed, tiled, twidth, theight
        );
    }

    return comp;
}


GraphicComp* OvImportCmd::PGM_Image (istream& in, boolean ascii) {
    GraphicComp* comp = nil;
    OverlayRaster* raster = PGM_Raster(in, ascii);
    if (raster != nil) {
        comp = new RasterOvComp(new OverlayRasterRect(raster));
    }
    return comp;
}


OverlayRaster* OvImportCmd::PGM_Raster (istream& in, boolean ascii) {
  
    char* buffer;
    in.gets(&buffer);

    do {  // CREATOR and other comments
        in.gets(&buffer);
    } while (buffer[0] == '#');

    int nrows, ncols;
    if (sscanf(buffer, "%d %d", &ncols, &nrows)==1) {
          in.gets(&buffer);
          sscanf(buffer, "%d", &nrows);
    }
    in.gets(&buffer);
    int maxval;
    sscanf(buffer, "%d", &maxval);

    OverlayRaster* raster;
    if (RasterOvComp::UseGrayRaster()) {
      if (maxval <= 255)
	raster = new GrayRaster(ncols, nrows);
      else if (maxval <= 65535)
	raster = new GrayRaster(ncols, nrows, AttributeValue::UShortType);
    } else
      raster = new OverlayRaster(ncols, nrows);

    if (AttributeValue::is_char(raster->value_type())) {
      for (int row = nrows - 1; row >= 0; --row) {
        for (int column = 0; column < ncols; ++column) {
	  if (ascii) {
	    int byte;
	    in >> byte;
	    raster->graypoke(column, row, (unsigned int)byte);
	  } else {
	    unsigned char byte;
	    in.get(byte);
	    raster->graypoke(column, row, (unsigned int)byte);
	  }
        }
      }
    } else {
      
      for (int row = nrows - 1; row >= 0; --row) {
        for (int column = 0; column < ncols; ++column) {
	  union {
	    unsigned char bytes[2];
	    unsigned short word;
	  } pixval;  

	  if (ascii) 
	    in >> pixval.word;
	  else {
	    in.get(pixval.bytes[0]);
	    in.get(pixval.bytes[1]);
	  }
	    
	  raster->graypoke(column, row, (unsigned int)pixval.word);
	}
      }
    }
  
    if (raster) raster->flush();
    return raster;
}


GraphicComp* OvImportCmd::Portable_Image_Tiled(
    PortableImageHelper* pih, const char* pathname, int twidth, 
    int theight, int width, int height, boolean compressed, boolean tiled
) {
    GraphicComp* group = new OverlaysComp;
    OverlayRaster* raster;

    int xbeg, xend, ybeg, yend;
    int rcount = 0;

    TileIterator it(twidth, theight, width, height);

    while(!it.Done()) {

        it.Step(xbeg, xend, ybeg, yend);

        raster = pih->create_raster(xend-xbeg+1, yend-ybeg+1);

        OverlayRasterRect* rr = new OverlayRasterRect(raster);
        rr->xbeg(xbeg);
        rr->xend(xend);
        rr->ybeg(ybeg);
        rr->yend(yend);

        Transformer* t = new Transformer(
            1, 0, 0, 1, xbeg, ybeg
        );
        rr->SetTransformer(t);
        Unref(t);

        group->Append(new RasterOvComp(rr, pathname));
        rcount++;

    }

    if (rcount == 1) {   // don't want groups of one
        Iterator i;
        group->First(i);
        GraphicComp* c = group->GetComp(i);
        group->Remove(i);
        delete group;
        group = c;
    }

    return group;
}


const char* OvImportCmd::Create_Tiled_File(
    const char* pifile, const char* tiledfile, int twidth, int theight
) {
    PortableImageHelper* pih;
    int width, height;
    boolean compressed;
    boolean tiled;

    int xbeg, xend, ybeg, yend;
    int ignore;

    if ((twidth < 10) || (theight < 10)) {
        return "tile dimensions must be >= 10";
    }

    FILE* infile = Portable_Raster_Open(
        pih, pifile, -1, width, height, compressed, tiled, ignore,
        ignore
    );
  
    if (!infile) {
        return "error opening the input file";
    }

    if (tiled) {
        closef(infile, compressed);
        return "file already tiled";
    }

    FILE* outfile = fopen(tiledfile, "w");

    if (!outfile) {
        closef(infile, compressed);
        return "error opening the output file";
    }

    fprintf(outfile, pih->magic());
    // insert a comment to include the tile size
    fprintf(outfile,"# tile %d %d\n", twidth, theight);
    fprintf(outfile,"%d %d\n", width, height);
    fprintf(outfile,"255\n");

    TileIterator it(twidth, theight, width, height);

    // This is a slow way of reading the file since we are buffering lots
    // of data that we are throwing out.  But we do this only once per file.
 
    long data = ftell(infile);
    int bpp = pih->bytes_per_pixel(); 

    while(!it.Done()) {
        it.Step(xbeg, xend, ybeg, yend);

        fseek(infile, long(width)*bpp*((height-1)-yend) + data, 0);

	int fseek_amt = 0;
	for (int row = yend; row >= ybeg; --row) {
            fseek_amt += xbeg*bpp;
	    if (fseek_amt>0)
		fseek(infile, fseek_amt, 1);
	    
	    for (int column = xbeg; column <= xend; ++column) {
                pih->read_write_pixel(infile, outfile);
	    }
	    fseek_amt = (width-xend-1)*bpp;
	}
    }

    fclose(outfile);
    closef(infile, compressed);

    return nil;
}


GraphicComp* OvImportCmd::Create_Comp(
    PortableImageHelper* pih, FILE* file, const char* pathname, int width, 
    int height, boolean compressed, boolean tiled, int twidth, int theight
) {
    GraphicComp* comp = nil;

    if (tiled || Tiling(twidth, theight)) {
        closef(file, compressed);
        comp = Portable_Image_Tiled(
            pih, pathname, twidth, theight, width, height, 
            compressed, tiled
        );
    }
    else {
        OverlayRaster* raster = PI_Raster_Read(
            pih, file, width, height, compressed, tiled, false, 
            nil, -1, -1, -1, -1
        );
        if (raster) {
            comp = new RasterOvComp(new OverlayRasterRect(raster), pathname);
        }
    }

    return comp;
}


GraphicComp* OvImportCmd::PPM_Image (const char* pathname) {
    GraphicComp* comp = nil;
    int width, height;
    boolean compressed;
    boolean tiled;
    int twidth, theight;
    PortableImageHelper* pih;

    FILE* file = Portable_Raster_Open(
        pih, pathname, 1, width, height, compressed, tiled, twidth, theight
    );
    if (file) {
        comp = Create_Comp(
            pih, file, pathname, width, height, compressed, tiled, twidth, theight
        );
    }

    return comp;
}


FILE* OvImportCmd::Portable_Raster_Open(
    PortableImageHelper*& pih, const char* pathname, int expect_ppm, 
    int& ncols, int& nrows, boolean& compressed, boolean& tiled, int& twidth, 
    int& theight
) {
    FILE* file = fopen(pathname, "r");
    file = CheckCompression(file, pathname, compressed);
  
    tiled = false;

    if (file != nil) {
        char buffer[BUFSIZ];
	fgets(buffer, BUFSIZ, file);

        int is_ppm = (!strcmp("P6\n", buffer) || !strcmp("P3\n", buffer));
        int is_pgm = !strcmp("P5\n", buffer) || !strcmp("P2\n", buffer);
        int is_ascii = !strcmp("P2\n", buffer) || !strcmp("P3\n", buffer);



	if (
            ( !is_ppm && !is_pgm ) ||
            ( is_ppm && (expect_ppm == 0)) || 
            ( is_pgm && (expect_ppm == 1))
        ) {
            closef(file, compressed);
	    return nil;
	}
        else 
            pih = is_pgm ? new PGM_Helper(is_ascii) : new PPM_Helper(is_ascii);


	fgets(buffer, BUFSIZ, file);                  // check for tiled file
        if (!strncmp(buffer, "# tile", 6)) {
            tiled = true;
	    int check = sscanf(buffer+7, "%d %d", &twidth, &theight);
            if ( check != 2 ) {
                closef(file, compressed);
	        return nil; 
            }    
        }

        while (buffer[0] == '#') {               // CREATOR and other comments
	    fgets(buffer, BUFSIZ, file);                
	}

        if (sscanf(buffer, "%d %d", &ncols, &nrows)==1) {
            fgets(buffer, BUFSIZ, file);
            sscanf(buffer, "%d", &nrows);
        }

        fgets(buffer, BUFSIZ, file);                 
        int maxval;
        sscanf(buffer, "%d", &maxval);
        if (maxval != 255 && maxval != 65535) {
            closef(file, compressed);
	    return nil; 
        }
        pih->maxval(maxval);
    }

    return file;
}


void OvImportCmd::PI_Tiled_Read(
    PortableImageHelper* pih, FILE* file, OverlayRaster* raster, int width, 
    int height, int xbeg, int xend, int ybeg, int yend
) {
    long fseek_amt = 
        ( (long) xbeg * (yend + 1) + ( (width - xbeg) * ybeg ) ) * 
        pih->bytes_per_pixel();


    fseek(file, fseek_amt, 1);

    for (int row = yend; row >= ybeg; --row) {
	for (int column = xbeg; column <= xend; ++column) {
            pih->read_poke(raster, file, column-xbeg, row-ybeg);
	}
    }
}


void OvImportCmd::PI_Normal_Read(
    PortableImageHelper* pih, FILE* file, OverlayRaster* raster, int ncols, 
    int nrows, int xbeg, int xend, int ybeg, int yend
) {
    int bpp = pih->bytes_per_pixel();

    if (nrows-1>yend)
	fseek(file, long(ncols)*bpp*((nrows-1)-yend), 1);

    int fseek_amt = 0;
    for (int row = yend; row >= ybeg; --row) {

	fseek_amt += xbeg*bpp;
	if (fseek_amt>0)
	    fseek(file, fseek_amt, 1);
	    
	for (int column = xbeg; column <= xend; ++column) {
            pih->read_poke(raster, file, column-xbeg, row-ybeg);
	}
	fseek_amt = (ncols-xend-1)*bpp;
    }
}


OverlayRaster* OvImportCmd::PI_Raster_Read(
    PortableImageHelper* pih, FILE* file, int ncols, int nrows, 
    boolean compressed, boolean tiled, boolean delayed, OverlayRaster* raster,
    IntCoord xbeg, IntCoord xend, IntCoord ybeg, IntCoord yend
) {
    xbeg = xbeg < 0 ? 0 : min(xbeg, ncols-1);
    xend = xend < 0 ? ncols-1 : min(xend, ncols-1);
    ybeg = ybeg < 0 ? 0 : min(ybeg, nrows-1);
    yend = yend < 0 ? nrows-1 : min(yend, nrows-1);

    if (!raster) 
	raster = pih->create_raster(xend-xbeg+1, yend-ybeg+1);
    else
	raster->init_rep(xend-xbeg+1, yend-ybeg+1);

    if (!delayed) {
        if (tiled) {
            PI_Tiled_Read(
                pih, file, raster, ncols, nrows, xbeg, xend, ybeg, yend
            );
        }
        else {
            PI_Normal_Read(
                pih, file, raster, ncols, nrows, xbeg, xend, ybeg, yend
            );
        }
    }

    closef(file,compressed);
    raster->flush();
    return raster;
}


OverlayRaster* OvImportCmd::PPM_Raster(
    const char* pathname, boolean delayed, OverlayRaster* raster,
    IntCoord xbeg, IntCoord xend, IntCoord ybeg, IntCoord yend
) {
    int width, height;
    boolean compressed;
    boolean tiled;
    int ignore;
    PortableImageHelper* pih;

    FILE* file = Portable_Raster_Open(
        pih, pathname, 1, width, height, compressed, tiled, ignore, ignore
    );

    if (file) {
        raster = PI_Raster_Read(
            pih, file, width, height, compressed, tiled, delayed, raster, 
            xbeg, xend, ybeg, yend
        );
    }
    else {
        raster = nil;
    }

    return raster;
}


GraphicComp* OvImportCmd::PPM_Image (istream& in, boolean ascii) {
    GraphicComp* comp = nil;
    int width, height;
    OverlayRaster* raster = PPM_Raster(in, ascii);
    if (raster) {
      comp = new RasterOvComp(new OverlayRasterRect(raster));
    }
    return comp;
}


OverlayRaster* OvImportCmd::PPM_Raster (istream& in, boolean ascii) 
{
    char* buffer;
    in.gets(&buffer);

    do { // CREATOR and other comments
        in.gets(&buffer);
    } while (buffer[0] == '#');
    int nrows, ncols;
    sscanf(buffer, "%d %d", &ncols, &nrows);
    in.gets(&buffer);
    int maxval;
    sscanf(buffer, "%d", &maxval);
    if (maxval != 255) {
        return nil; 
    }

    OverlayRaster* raster = new OverlayRaster(ncols, nrows);
  
    for (int row = nrows - 1; row >= 0; --row) {
        for (int column = 0; column < ncols; ++column) {
          if (ascii) {
            int red, green, blue;
            in >> red >> green >> blue;
            raster->poke(column, row,
               float(red)/0xff, float(green)/0xff, float(blue)/0xff, 1.0);
          } else {
            unsigned char red, green, blue;
            in.get(red);
            in.get(green);
            in.get(blue);
            raster->poke(column, row,
               float(red)/0xff, float(green)/0xff, float(blue)/0xff, 1.0);
          }
        }
    }
    
    if (raster) 
        raster->flush();
    return raster;
}

GraphicComp* OvImportCmd::PNM_Image_Filter (istream& in, const char* filter)
{
  int outfd = Pipe_Filter(in, filter);

  filebuf fbuf;
  fbuf.attach(outfd);
  istream in2(&fbuf);
  GraphicComp* comp = PNM_Image(in2);
  if(close(outfd)==-1)
    cerr << "error in parent closing last end of the pipes\n";
  return comp;
}

int OvImportCmd::Pipe_Filter (istream& in, const char* filter)
{
  int pfdout[2], pfdin[2];

  cerr << "input filtered by " << filter << "\n";

  if (pipe(pfdout)==-1 || pipe(pfdin)==-1) 
    cerr << "error opening pipes\n";

  /* open child to write to filter */
  switch(fork()) {
  case -1:
    cerr << "error in fork\n";
    break;
  case 0:

    /* open grandchild to execute filter */
    switch(fork()) {
    case -1:
      cerr << "error in second fork\n";
      break;
    case 0:
      if(close(0)==-1)
	cerr << "error in grandchild close of 0\n";
      if (dup(pfdout[0]) != 0)
	cerr << "error in grandchild dup of pfdout[0]\n";
      if(close(1)==-1)
	cerr << "error in grandchild close of 1\n";
      if (dup(pfdin[1]) != 1)
	cerr << "error in grandchild dup of pfdint[1]\n";
      if(close(pfdout[0])==-1 || close(pfdout[1])==-1 ||
	 close(pfdin[0])==-1 || close(pfdin[1])==-1)
	cerr << "error in grandchild close of pipes\n";
      execlp("sh", "sh", "-c", filter, NULL);     
      cerr << "error in ever getting here after execlp\n";
      exit(-1);
    }
    
    if(close(pfdout[0])==-1 || 
       close(pfdin[0])==-1 || close(pfdin[1])==-1)
      cerr << "error in child close of three out of 4 pipes\n";
    filebuf fbuf;
    fbuf.attach(pfdout[1]);
    ostream out(&fbuf);
    char buffer[BUFSIZ];
    while (!in.eof() && in.good()) {
      in.read(buffer, BUFSIZ);
      if (!in.eof() || in.gcount())
	out.write(buffer, in.gcount());
    }
    out.flush();
    if(close(pfdout[1])==-1)
      cerr << "error in child closing its output pipe\n";
    int status;
    if(wait(&status) ==-1)
      cerr << "error in child waiting for grandchild\n";
    exit(0);
  }

  if(close(pfdout[0])==-1 || close(pfdin[1]) == -1 || 
     close(pfdout[1])==-1)
    cerr << "error in parent closing unused three ends of the pipes\n";
  return pfdin[0];
}

GraphicComp* OvImportCmd::PNM_Image (istream& in, const char* creator) 
{

  FileType filetype;
  if (!creator) creator = ReadCreator(in, filetype);


  if (strncmp(creator, "PPM", 3)==0) {
    boolean asciiflag = creator[3] == 'A';
    return PPM_Image(in, asciiflag);
  } else
  if (strncmp(creator, "PGM", 3)==0) {
    boolean asciiflag =creator[3] == 'A';
    return PGM_Image(in, asciiflag);
  } else
  if (strncmp(creator, "PBM", 3)==0) {
    return PBM_Image(in);
  } else
    return nil;
}


GraphicComp* OvImportCmd::XBitmap_Image (const char* pathname) {
    GraphicComp* comp = nil;
    Bitmap* bitmap = XBitmap_Bitmap(pathname);

    if (bitmap != nil) {
	comp = new StencilOvComp(
	    new UStencil(bitmap, bitmap, stdgraphic), pathname
	);
    }

    return comp;
}

Bitmap* OvImportCmd::XBitmap_Bitmap (const char* pathname) {
    Bitmap* bitmap = nil;
    FILE* file = fopen(pathname, "r");

    if (file != nil)
        bitmap = Bitmap::open(pathname);

    fclose(file);
    bitmap->flush();
    return bitmap;
}

GraphicComp* OvImportCmd::PBM_Image (const char* pathname) {
    GraphicComp* comp = nil;
    Bitmap* bitmap = PBM_Bitmap(pathname);

    if (bitmap != nil) {
	comp = new StencilOvComp(
	    new UStencil(bitmap, bitmap, stdgraphic), pathname
	);
    }

    return comp;
}

Bitmap* OvImportCmd::PBM_Bitmap (const char* pathname) {
    Bitmap* bitmap = nil;
    FILE* file = fopen(pathname, "r");
    boolean compressed;
    file = CheckCompression(file, pathname, compressed);

    if (file != nil) {
        char buffer[BUFSIZ];
	fgets(buffer, BUFSIZ, file);

	if (strcmp("P4\n", buffer) != 0 && 
	    strcmp("P1\n", buffer) != 0) {              // magic number
	    if (compressed) 
		pclose(file); 
	    else 
		fclose(file);
	    return nil;
	}
	boolean asciiflag = strcmp("P1\n", buffer)==0;
	do {                                            // CREATOR and other comments
	    fgets(buffer, BUFSIZ, file);                
	} while (buffer[0] == '#');
	int nrows, ncols;
        if (sscanf(buffer, "%d %d", &ncols, &nrows)==1) {
          fgets(buffer, BUFSIZ, file);
          sscanf(buffer, "%d", &nrows);
        }
        void* nilpointer = nil;
        bitmap = new Bitmap(nilpointer, ncols, nrows);

	int byte;

	if (!asciiflag) {
	  for (int row = nrows - 1; row >= 0; --row) {
	    int mask = 128;
            for (int column = 0; column < ncols; ++column) {
	      if (mask == 128) byte = getc(file);
	      boolean bit = byte & mask;
	      mask = mask >> 1;
	      if (mask == 0) mask = 128;
	      bitmap->poke(bit, column, row);
            }
	  }
	} else {
	  int column = 0;
	  int row = 0;
	  while (row < nrows) {
	    while (column < ncols) {
	      int bit;
	      if(fscanf(file, "%d", &bit)==1)
		bitmap->poke(bit, column, nrows-row-1);
	      column++;
	    }
	    column=0;
	    row++;
	  }
	}
    }
    
    if (compressed) {
      pclose(file);
    } else {
      fclose(file);
    }
    bitmap->flush();
    return bitmap;
}

GraphicComp* OvImportCmd::PBM_Image (istream& in) {
    GraphicComp* comp = nil;
    Bitmap* bitmap = PBM_Bitmap(in);

    if (bitmap != nil) {
	comp = new StencilOvComp(
	    new UStencil(bitmap, bitmap, stdgraphic));
    }

    return comp;
}

Bitmap* OvImportCmd::PBM_Bitmap (istream& in) {
    Bitmap* bitmap = nil;
  
    char* buffer;
    in.gets(&buffer);

    boolean asciiflag = strncmp("P1", buffer, 2) == 0;
  
    do { // CREATOR and other comments
        in.gets(&buffer);
    } while (buffer[0] == '#');

    int nrows, ncols;
    if (sscanf(buffer, "%d %d", &ncols, &nrows)==1) {
          in.gets(&buffer);
          sscanf(buffer, "%d", &nrows);
    }
    void* nilpointer = nil;
    bitmap = new Bitmap(nilpointer, ncols, nrows);

    if (!asciiflag) {
      int byte;
      for (int row = nrows - 1; row >= 0; --row) {
        int mask = 128;
        for (int column = 0; column < ncols; ++column) {
	  if (mask == 128) byte = in.get();
	  boolean bit = byte & mask;
	  mask = mask >> 1;
	  if (mask == 0) mask = 128;
	  bitmap->poke(bit, column, row);
        }
      }
    } else {
      int column = 0;
      int row = 0;
      while (row < nrows) {
	while (column < ncols) {
	  int bit;
	  in >> bit;
	  if(!in.eof() || in.gcount())
	    bitmap->poke(bit, column, nrows-row-1);
	  column++;
	}
	column=0;
	row++;
      }
    }
      
    bitmap->flush();
    return bitmap;
}

