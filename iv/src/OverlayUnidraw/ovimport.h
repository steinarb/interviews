/*
 * Copyright (c) 1996-1997 Vectaport Inc., R.B. Kissh & Associates
 * Copyright (c) 1994-1995 Vectaport Inc.
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
 * OvImportCmd - a command for importing graphical objects
 */

#ifndef ov_import_h
#define ov_import_h

#include <Unidraw/Commands/command.h>
#include <stdio.h>
#include <sys/types.h>

class ImportChooser;
class OverlayRaster;
class Raster;
class Bitmap;
class PortableImageHelper;

class OvImportCmd : public Command {
public:
    enum FileType { UnknownFile, RasterFile, PostscriptFile, IvtoolsFile, CompressedFile };

    OvImportCmd(Editor* = nil, ImportChooser* = nil);
    OvImportCmd(ControlInfo* = nil, ImportChooser* = nil);
    virtual ~OvImportCmd();
    void Init(ImportChooser*);

    void instream(istream*);
    void pathname(const char*, boolean popen=false);
    const char* pathname() { return path_; }
    void preserve_selection(boolean);

    virtual Command* Copy();
    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);

    virtual void Execute();
    virtual boolean Reversible();
    virtual GraphicComp* PostDialog();

    virtual GraphicComp* Import(const char*);
    virtual GraphicComp* Import(istream&);
    virtual GraphicComp* Import(istream&, boolean& empty);

    static GraphicComp* TIFF_Image(const char*);
    static OverlayRaster* TIFF_Raster(const char*);

    static GraphicComp* PGM_Image(const char*);
    static OverlayRaster* PGM_Raster(
	const char*, boolean delayed = false, OverlayRaster* = nil,
	IntCoord xbeg=-1, IntCoord xend=-1, IntCoord ybeg=-1, IntCoord yend=-1
    );
    static GraphicComp* PGM_Image(istream&, boolean ascii=false);
    static OverlayRaster* PGM_Raster(istream&, boolean ascii=false);

    static GraphicComp* PPM_Image(const char*);
    static OverlayRaster* PPM_Raster(
	const char*, boolean delayed = false, OverlayRaster* = nil,
	IntCoord xbeg=-1, IntCoord xend=-1, IntCoord ybeg=-1, IntCoord yend=-1
    );
    static GraphicComp* PPM_Image(istream&, boolean ascii=false);
    static OverlayRaster* PPM_Raster(istream& in, boolean ascii=false);

    static GraphicComp* PNM_Image(istream&, const char* creator = nil);
    static GraphicComp* PNM_Image_Filter(istream&, const char* filter = nil);

    static int Pipe_Filter(istream& in, const char* filter);

    static boolean Tiling(int& width, int& height);

    static GraphicComp* XBitmap_Image(const char*);
    static Bitmap* XBitmap_Bitmap(const char*);
    static GraphicComp* PBM_Image(const char*);
    static Bitmap* PBM_Bitmap(const char*);
    static GraphicComp* PBM_Image(istream&);
    static Bitmap* PBM_Bitmap(istream&);

    static const char* ReadCreator(const char* pathname);
    static const char* ReadCreator(istream& in, FileType& type);

    static FILE* CheckCompression(
	FILE* file, const char *pathname, boolean& compressed);

// private:
    static OverlayRaster* PI_Raster_Read(
        PortableImageHelper*, FILE* file, int ncols, int nrows, 
        boolean compressed, boolean tiled, boolean delayed, 
        OverlayRaster* raster, IntCoord xbeg, IntCoord xend, IntCoord ybeg, 
        IntCoord yend
    );

    static void PI_Normal_Read(
        PortableImageHelper*, FILE* file, OverlayRaster* raster, int ncols, 
        int nrows, int xbeg, int xend, int ybeg, int yend
    );

    static void PI_Tiled_Read(
        PortableImageHelper*, FILE* file, OverlayRaster* raster, int ncols, 
        int nrows, int xbeg, int xend, int ybeg, int yend
    );

    static GraphicComp* Portable_Image_Tiled(
        PortableImageHelper*, const char* pathname, int twidth, int theight, 
        int width, int height, boolean compressed, boolean tiled
    );

    static FILE* Portable_Raster_Open(
        PortableImageHelper*&, const char* pathname, int ppm, int& ncols, 
        int& nrows, boolean& compressed, boolean& tiled, int& twidth, 
        int& theight
    );

    static const char* Create_Tiled_File(
        const char* ppmfile, const char* tilefile, int twidth, int theight
    );

    static GraphicComp* Create_Comp(
        PortableImageHelper* pih, FILE*, const char* pathname, int width, 
        int height, boolean compressed, boolean tiled, int twidth, 
        int theight
    );

    void is_url(boolean flag) { _is_url = flag; }
    boolean is_url() { return _is_url; }

protected:
    ImportChooser* chooser_;
    istream* inptr_;
    char* path_;
    boolean popen_;
    boolean preserve_selection_;
    boolean _is_url;
};



class PortableImageHelper {
public:
    PortableImageHelper(boolean is_ascii=false) 
      { _is_ascii = is_ascii; _maxval = 255;}
    virtual boolean ppm() = 0;
    virtual int bytes_per_pixel() = 0;
    virtual void read_write_pixel( FILE* in, FILE* out ) = 0;
    virtual const char* magic() = 0;
    virtual void read_poke( OverlayRaster*, FILE*, u_long x, u_long y ) = 0;
    virtual OverlayRaster* create_raster( u_long w, u_long h ) = 0;

    boolean is_ascii() { return _is_ascii; }
    void maxval(int maxv) { _maxval = maxv; }
    int maxval() { return _maxval; }
protected:
    boolean _is_ascii;
    int _maxval;
};


class PGM_Helper : public PortableImageHelper {
public:
    PGM_Helper(boolean is_ascii=false);
    virtual boolean ppm();
    virtual int bytes_per_pixel();
    virtual void read_write_pixel( FILE* in, FILE* out );
    virtual const char* magic();
    virtual void read_poke( OverlayRaster*, FILE*, u_long x, u_long y );
    virtual OverlayRaster* create_raster( u_long w, u_long h );
};

class PPM_Helper : public PortableImageHelper {
public:
    PPM_Helper(boolean is_ascii=false);
    virtual boolean ppm();
    virtual int bytes_per_pixel();
    virtual void read_write_pixel( FILE* in, FILE* out );
    virtual const char* magic();
    virtual void read_poke( OverlayRaster*, FILE*, u_long x, u_long y );
    virtual OverlayRaster* create_raster( u_long w, u_long h );
};


#endif


