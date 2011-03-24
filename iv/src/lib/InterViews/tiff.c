/*
 * Copyright (c) 1991 Stanford University
 * Copyright (c) 1991 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and 
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Stanford and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Stanford and Silicon Graphics.
 * 
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 *
 * IN NO EVENT SHALL STANFORD OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
 * OF THIS SOFTWARE.
 */

#include <InterViews/color.h>
#include <InterViews/raster.h>
#include <InterViews/tiff.h>
#include <TIFF/tiffio.h>
#include <math.h>

class TIFFRasterImpl {
private:
    friend class TIFFRaster;

    TIFF* tif_;
    unsigned short bytes_per_row_;
    unsigned short bits_per_sample_;
    unsigned short samples_per_pixel_;
    unsigned short max_sample_value_;
    unsigned long height_;
    unsigned long width_;
    unsigned int colormapsize_;
    unsigned char* imagedata_;
    double* dred_;
    double* dgreen_;
    double* dblue_;

    TIFFRasterImpl();
    ~TIFFRasterImpl();

    Raster* load(const char* filename, boolean make_gray);
    unsigned short identify();
    boolean build_colormap(unsigned short tiftype);
    boolean photometric(int tifftag);
    boolean gray_map(unsigned short tiftype);
    boolean load_pallette();
    boolean is_8bitmap();
    void scale_map(float max);
    void color_to_gray();
    boolean load_rgb(unsigned short planarconfig);
    Raster* colormap_raster();
    Raster* rgb_raster();
};

TIFFRasterImpl::TIFFRasterImpl() {
    imagedata_ = nil;
    dred_ = nil;
    dgreen_ = nil;
    dblue_ = nil;
}

TIFFRasterImpl::~TIFFRasterImpl() {
    delete imagedata_;
    delete dred_;
    delete dgreen_;
    delete dblue_;
}

Raster* TIFFRaster::load(const char* filename, boolean make_gray) {
    TIFFRasterImpl impl;
    return impl.load(filename, make_gray);
}

Raster* TIFFRasterImpl::load(const char* filename, boolean make_gray) {
    tif_ = TIFFOpen(filename, "r");
    if (tif_ == nil) {
	return nil;
    }

    TIFFGetField(tif_, TIFFTAG_IMAGEWIDTH, &width_);
    TIFFGetField(tif_, TIFFTAG_IMAGELENGTH, &height_);

    if (!TIFFGetField(tif_, TIFFTAG_BITSPERSAMPLE, &bits_per_sample_)) {
	bits_per_sample_ = 1;
    }
    if (bits_per_sample_ != 8) {
	TIFFClose(tif_);
	return nil;
    }

    if (!TIFFGetField(tif_, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel_)) {
        samples_per_pixel_ = 1;
    }

    unsigned short planarconfig;
    if (!TIFFGetField(tif_, TIFFTAG_PLANARCONFIG, &planarconfig)) {
        planarconfig=PLANARCONFIG_CONTIG;
    }

    if (!TIFFGetField(tif_, TIFFTAG_MAXSAMPLEVALUE, &max_sample_value_)) {
        max_sample_value_ = 0xffff;
    }
    bytes_per_row_ = TIFFScanlineSize(tif_);
    colormapsize_ = 1 << bits_per_sample_;
    
    unsigned short tifinterpretation = identify();
    if (!build_colormap(tifinterpretation)) {
	TIFFClose(tif_);
	return nil;
    }

    Raster* r = nil;
    switch (bits_per_sample_ * samples_per_pixel_) {
    case 8:
        if (load_pallette()) {
	    if (make_gray) {
		color_to_gray();
	    }
	    r = colormap_raster();
	}
        break;
    case 24:
    case 32:
        if (load_rgb(planarconfig)) {
	    r = rgb_raster();
        }
        break;
    }
    TIFFClose(tif_);
    return r;
}

unsigned short TIFFRasterImpl::identify() {
    unsigned short s;
    unsigned short* redmap, * greenmap, * bluemap;
    
    if (!TIFFGetField(tif_, TIFFTAG_PHOTOMETRIC, &s)) {
        if (samples_per_pixel_ != 1) {
            s = PHOTOMETRIC_RGB;
        } else if (bits_per_sample_ == 1) {
            s = PHOTOMETRIC_MINISBLACK;
    	} else if (
	    TIFFGetField(tif_, TIFFTAG_COLORMAP, &redmap, &greenmap, &bluemap)
	) {
            s = PHOTOMETRIC_PALETTE;
        } else {
            s = PHOTOMETRIC_MINISBLACK;
        }
    }
    return s;
}

boolean TIFFRasterImpl::build_colormap(unsigned short tiftype) {
    dred_ = new double[colormapsize_];
    dgreen_ = new double[colormapsize_];
    dblue_ = new double[colormapsize_];
    switch (tiftype) {
    case PHOTOMETRIC_PALETTE:
        if (!photometric(TIFFTAG_COLORMAP)) return false;
        break;
    case PHOTOMETRIC_RGB:
        // if (!photometric(TIFFTAG_COLORRESPONSECURVE)) return false;
        break;
    case PHOTOMETRIC_MINISWHITE:
        if (!gray_map(PHOTOMETRIC_MINISWHITE)) return false;
        break;
    case PHOTOMETRIC_MINISBLACK:
        if (!gray_map(PHOTOMETRIC_MINISBLACK)) return false;
        break;
    default:
        return false;
    }
    return true;
}

boolean TIFFRasterImpl::load_pallette() {
    unsigned char* c = new unsigned char[height_ * bytes_per_row_];
    imagedata_ = c;
    for (unsigned int j = 0; j < height_; j++) {
        if (TIFFReadScanline(tif_, c, j, 0) < 0) {
            return false;
        }
        c += bytes_per_row_;
    }
    return true;
}

boolean TIFFRasterImpl::load_rgb(unsigned short planarconfig) {
    unsigned char* line =
	new unsigned char[height_ * width_ * samples_per_pixel_];
    imagedata_ = line;
    if (planarconfig == PLANARCONFIG_CONTIG) {
        for (unsigned int i = 0; i < height_; i++) {
            if (TIFFReadScanline(tif_, line, i, 0) < 0) {
                return false;
            } 
            line += bytes_per_row_;
        }
    } else {
        unsigned char *ptr, *last;
        unsigned char *scanline = new unsigned char[bytes_per_row_];
        
        for (unsigned int j = 0; j < 3; j++) {
            for (unsigned int i = 0; i < height_; i++) {
                if (TIFFReadScanline(tif_, scanline, i, j) < 0) {
                    delete scanline;
                    return false;
                }
                ptr = scanline;
                line = imagedata_ + (samples_per_pixel_ * width_ * i) + j;
                last = line + samples_per_pixel_ * width_;
                while (line < last) {
                    *line = *ptr++;
                    line += samples_per_pixel_;
                }
            }
        }
        delete scanline;
    }
    return true;
}

Raster* TIFFRasterImpl::colormap_raster() {
    Raster* r = new Raster(width_, height_);
    unsigned char *c = imagedata_;
    for (long i = height_ - 1; i >= 0; i--) {
	for (long j = 0; j < width_; j++) {
	    r->poke(
		j, i,
		ColorIntensity(dred_[*c] / double(0xffff)),
		ColorIntensity(dgreen_[*c] / double(0xffff)),
		ColorIntensity(dblue_[*c] / double(0xffff)),
		1.0
	    );
	    c++;
	}
    }
    return r;
}

Raster* TIFFRasterImpl::rgb_raster() {
    Raster* r = new Raster(width_, height_);
    unsigned char *c = imagedata_;
    for (long i = height_ - 1; i >= 0; i--) {
	for (long j = 0; j < width_; j++) {
	    r->poke(
		j, i,
		ColorIntensity(float(*c) / float(0xff)),
		ColorIntensity(float(*(c+1)) / float(0xff)),
		ColorIntensity(float(*(c+2)) / float(0xff)),
		1.0
	    );
	    c += samples_per_pixel_;
	}
    }
    return r;
}

void TIFFRasterImpl::color_to_gray() {
    double temp;
    
    for (int i = 0; i < colormapsize_ ; i++){
    	temp = .299 * dred_[i] + .587 * dgreen_[i] + .114 * dblue_[i];
        dred_[i] = dgreen_[i] = dblue_[i] = temp;
    }
}

static float Scale(float x, float y) {
    return (x * 0xffff / y);
}

static int Round(float x) {
    return ((int)(x + .5));
}

boolean TIFFRasterImpl::is_8bitmap() {
    for (int i = 0; i < colormapsize_; i++) {
        if (dred_[i] > 255 || dgreen_[i] > 255 || dblue_[i] > 255) {
            return false;
	}
    }
    return true;
}

void TIFFRasterImpl::scale_map(float max) {
    for (int i = 0; i < colormapsize_; i++) {
	dred_[i] = (double) Scale(dred_[i], max);
	dgreen_[i] = (double) Scale(dgreen_[i], max);
	dblue_[i] = (double) Scale(dblue_[i], max);
    }
}

boolean TIFFRasterImpl::photometric(int tifftag) {
    unsigned short* redmap, * greenmap, * bluemap;
    
    if (!TIFFGetField(tif_, tifftag, &redmap, &greenmap, &bluemap)) {
        for (int i = 0; i < colormapsize_; i++) {
            dred_[i] = dblue_[i] = dgreen_[i] =
                (double) Scale(i, colormapsize_ - 1);
        }
    } else {
	for (int i = 0; i < colormapsize_; i++) {
	    dred_[i] = (double) redmap[i];
	    dgreen_[i] = (double) greenmap[i];
	    dblue_[i] = (double) bluemap[i];
	}
	if (max_sample_value_ <= 0xff || is_8bitmap()) {
	    scale_map(0xff);
	}
    }
    return true;
}

boolean TIFFRasterImpl::gray_map(unsigned short tiftype) {
    double unitmap;
    unsigned short grayresponseunit, *graymap = NULL;
    
    if (!TIFFGetField(tif_, TIFFTAG_GRAYRESPONSECURVE, &graymap)) {
        if (tiftype == PHOTOMETRIC_MINISBLACK) {
            for (int i = 0; i < colormapsize_; i++) {
                dred_[i] = dgreen_[i] = dblue_[i] =
                    (i % 4 == 0) ?
			(unsigned short)Scale(i, colormapsize_ - 1) :
			dred_[i - 1];
            }
        } else {
            for (int i = 0; i < colormapsize_; i++) {
                dred_[i] = dgreen_[i] = dblue_[i] = (i % 4==0) ?
                    Scale(colormapsize_ - 1 - i, colormapsize_ - 1) :
		    dred_[i - 1];
            }
        }
    } else {
        if (!TIFFGetField(tif_, TIFFTAG_GRAYRESPONSEUNIT, &grayresponseunit)) {
            grayresponseunit = 2;
	}
        unitmap = (-1) * (pow(10, grayresponseunit));
        if (tiftype == PHOTOMETRIC_MINISBLACK) {
            dred_[0] = dgreen_[0] = dblue_[0] = 0;
            for (int i = 1; i < colormapsize_; i++) {
                dred_[i] = dgreen_[i] = dblue_[i] =
                    (0xffff * pow(10.0, (double) graymap[i]/unitmap));
            }
        } else {
            int i = colormapsize_ - 1;
            dred_[i] = dgreen_[i] = dblue_[i] = 0;
            for (i = 0; i < colormapsize_ - 1; i++) {
                dred_[i] = dgreen_[i] = dblue_[i]=
                    (0xffff * (1-pow(10.0, (double) graymap[i]/unitmap)));
            }
        }
    }
    return true;
}
