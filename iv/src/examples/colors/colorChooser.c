/* colorChooser.c:  
 *  a simple interviews application for searching through the red-green-blue
 *  color space of your workstation or terminal, and learning the name of
 *  any color that can be displayed on it.  if you invoke this program with
 *  exactly one argument, the name of a file like the standard X
 *  distribution's "rgb.txt", the program will sometimes be able to tell 
 *  you an English language name  for the current color.  If the current color
 *  is not sufficiently near to any of these named colors, the program will 
 *  give you a 6-digit hexadecimal string to name the color.  In either case, 
 *  the name can be used to specify X resource colors.
 *  
 *  some sample invocations:
 *     colorChooser   
 *     colorChooser  /usr/lib/X11/rgb.txt
 *     colorChooser  /local/X11R5/lib/X11/rgb.txt
 *     colorChooser  $OPENWINHOME/lib/rgb.txt
 *
 *  (the last three try to cover the range of places where the rgb.txt
 *   might be found on your workstation)
 *
 *  here's what the rgb.txt file actually looks like: 
 *
 *         255 250 240        FloralWhite
 *         253 245 230        OldLace
 *         250 240 230        linen
 *         250 235 215        AntiqueWhite
 *         255 239 213        PapayaWhip
 *
 *
 *  This program is based upon "cedit" by Dave Sternlicht of the MIT 
 *  X Consortium.  Many thanks to Dave, for cedit and for his frequent, 
 *  helpful  postings to comp.windows.interviews.  
 *
 *  This program was developed using InterViews 3.1beta3.
 *
 *  I wrote this program as a learning exercise.  I've met with a lot of
 *  frustration in learning InterViews.  There are no books, and
 *  sample source code is invariably cryptic.  I hope that this program
 *  reads easily;  if any one has questions, suggestions or complaints,
 *  please send them to me, pshannon@nrao.edu, and I'll do my best to help.
 *
 *
 *                          thumbnail sketch
 *
 *  This program demonstrates the interaction between 3 fundamental 
 *  iv classes:  Adjustable,  Adjuster, and Observer.   See Chapter 4 of
 *  the InterViews manual for a description of these classes.  I believe
 *  that these three classes are analagous to Smalltalk's Model-View-
 *  Controller: Adjustables (Models) are the data your program is 
 *  concerned with;  Adjusters (Controllers) manipulate the data; 
 *  Observers (Views) provide ways to view the data.  In this program,
 *  instances of these three classes are used like this:
 *
 *  Adjusters:    3 vertical scrollbars, one each for the red, green, and
 *                blue color components.
 *  Adjustables:  3 "ColorValue" objects, each of which stores the color
 *                intensity of the color component it represents.
 *  Observers: -  1 color patch, which watches the ColorValue objects,
 *                and displays the color described by them
 *             -  3 "ColorValueReadout" objects, each of which 
 *                watches one of the ColorValues, and displays the intensity 
 *                of its ColorValue as a number between 0 and 255, in a
 *                little box just above the related scrollbar
 *             -  1 "DynamicLabel", which watches the ColorValue objects
 *                and displays a name for the current color in an inset
 *                box above the color patch.
 *
 *
 *                          a little more detail
 *
 * - the 3 "ColorValue" adjustables, one for each of the color 
 *   components, form the heart of the program.  they accept
 *   scrolling messages from the scrollbars.  they (implicitly) send
 *   update messages to all of their attached observers when their
 *   notify function is called.  they don't appear on the screen, but they 
 *   store the current intensitity of the colors, and the rate at which they 
 *   change these values in response to messages from adjusters (the 
 *   scrollbars).  
 *      class ColorValue: public Adjustable 
 *
 * - 3 scrollbars, which subclass Adjuster, and send messages to the
 *   ColorValue adjustables.  they are produced by WidgetKit member
 *   functions, and are initialized with a single parameter each --
 *   a pointer to the adjustable they'll adjust.
 *
 * - each of the observers is attached to the ColorValue adjustables, so 
 *   that they can be notified whenever the  adjustables have changed.  
 *   they then query the ColorValue objects to find out what their current 
 *   values are, and update themselves  appropriately:
 *
 *      1. class ColorDisplay: public Patch, public Observer 
 *      2. class DynamicLabel: public MonoGlyph, public Observer 
 *      3. class ColorValueReadout: public MonoGlyph, public Observer 
 *
 *   note: the ColorDisplay object and the DynamicLabel object are both
 *   attached to all three of the ColorValue objects: they both need
 *   the value of all three color components in order to update themselves.
 *   but the ColorValueReadout objects are each attached to only
 *   one of the ColorValue objects -- the one which corresponds to the
 *   color component they're concerned with.
 *
 *   there's one more class in this application, which simply returns
 *   a color name when it's passed a triplet of red-green-blue values.
 *   it's declared in rgbNames.h, and defined in rgbNames.c.
 *
 *      class RGBNames {
 *      public:
 *        RGBNames ();  
 *        RGBNames (const char *filename);
 *        char *lookupName (int red, int green, int blue);
 *        };
 */
/*----------------------------------------------------------------------------*/
static char *SCCSid = "@(#)colorChooser.c	2.2 12/16/92 12:03:28";
/*----------------------------------------------------------------------------*/
#include <IV-look/button.h>
#include <IV-look/dialogs.h>
#include <IV-look/kit.h>
#include <IV-look/menu.h>
#include <IV-look/field.h>
#include <IV-look/telltale.h>
#include <InterViews/adjust.h>
#include <InterViews/background.h>
#include <InterViews/box.h>
#include <InterViews/color.h>
#include <InterViews/display.h> 
#include <InterViews/glue.h>
#include <InterViews/label.h>
#include <InterViews/layout.h>
#include <InterViews/patch.h>
#include <InterViews/session.h>
#include <InterViews/style.h>
#include <InterViews/window.h>
#include <OS/string.h>
#include <stdio.h>
#include <string.h>

#include "rgbNames.h"  
/*----------------------------------------------------------------------------*/
/* ColorValue is a subclass of Adjustable. Adjustables are broadly defined
 * as objects that "handle requests to modify their viewing area."
 * objects of the ColorValue class, however, are not seen on the screen.
 * each object of this class
 *    - records the current intensity of one of the three primary colors 
 *        (the application has three ColorValue objects, one each for 
 *         red, green and blue);
 *    - sets the bounds that the color intensity must stay within
 *    - is connnected to a scrollbar (an adjuster subclass), from which
 *      it receives a variety of scrolling messages
 *    - has observers attached that can be told (via Adjustable::notify)
 *      that it has changed.  
 * the values range from 0.0 to 1.0, which is the InterViews color range,
 * perhaps inherited from PostScript.  I'm more used to thinking of color
 * on workstation screens ranging from 0-255, so you'll see below that
 * ColorValueReadout objects scale the numbers into that range.
 */
/*----------------------------------------------------------------------------*/
class ColorValue: public Adjustable {
protected:
    ColorValue ();
public:
    ColorValue (Coord lower, Coord upper);
    virtual ~ColorValue ();

    virtual void lower_bound (Coord);
    virtual void upper_bound (Coord);
    virtual void current_value (Coord);
    virtual void scroll_incr (Coord);
    virtual void page_incr (Coord);

    virtual Coord lower (DimensionName) const;
    virtual Coord upper (DimensionName) const;
    virtual Coord length (DimensionName) const;
    virtual Coord cur_lower (DimensionName) const;
    virtual Coord cur_upper (DimensionName) const;
    virtual Coord cur_length (DimensionName) const;

    virtual void scroll_to (DimensionName, Coord position);
    virtual void scroll_forward (DimensionName);
    virtual void scroll_backward (DimensionName);
    virtual void page_forward (DimensionName);
    virtual void page_backward (DimensionName);
private:
    Coord curvalue_;
    Coord lower_;
    Coord span_;
    Coord scroll_incr_;
    Coord page_incr_;
};
/*----------------------------------------------------------------------------*/
ColorValue::ColorValue () 
{
    scroll_incr_ = 0.0;
    page_incr_ = 0.0;
}
/*----------------------------------------------------------------------------*/
ColorValue::ColorValue (Coord lower, Coord upper) 
{
  lower_ = lower;
  span_ = upper - lower;
  scroll_incr_ = span_ * 0.005;
  page_incr_ = span_ * 0.1;
  curvalue_ = (lower + upper) * 0.5;
}
/*----------------------------------------------------------------------------*/
ColorValue::~ColorValue () {}
void ColorValue::lower_bound (Coord c) {lower_ = c;}
void ColorValue::upper_bound (Coord c) {span_ = c - lower_;}
/*----------------------------------------------------------------------------*/
void ColorValue::current_value (Coord value) 
{
  curvalue_ = value;
  constrain (Dimension_X, curvalue_);
  notify (Dimension_X);
  notify (Dimension_Y);
}
/*----------------------------------------------------------------------------*/
void ColorValue::scroll_incr (Coord c) {scroll_incr_ = c;}
void ColorValue::page_incr (Coord c)   {page_incr_ = c;}
Coord ColorValue::lower      (DimensionName) const {return lower_;}
Coord ColorValue::upper      (DimensionName) const {return lower_ + span_;}
Coord ColorValue::length     (DimensionName) const {return span_;}
Coord ColorValue::cur_lower  (DimensionName) const {return curvalue_;}
Coord ColorValue::cur_upper  (DimensionName) const {return curvalue_;}
Coord ColorValue::cur_length (DimensionName) const {return 0;}
/*----------------------------------------------------------------------------*/
void ColorValue::scroll_to (DimensionName d, Coord position) 
{
  Coord p = position;
  constrain (d, p);
  if (p != curvalue_) {
     curvalue_ = p;
     notify (Dimension_X);
     notify (Dimension_Y);
     }
}
/*----------------------------------------------------------------------------*/
void ColorValue::scroll_forward (DimensionName d) 
{
  scroll_to (d, curvalue_ + scroll_incr_);
}
/*----------------------------------------------------------------------------*/
void ColorValue::scroll_backward (DimensionName d) 
{
  scroll_to(d, curvalue_ - scroll_incr_);
}
/*----------------------------------------------------------------------------*/
void ColorValue::page_forward(DimensionName d) 
{
  scroll_to(d, curvalue_ + page_incr_);
}
/*----------------------------------------------------------------------------*/
void ColorValue::page_backward(DimensionName d) 
{
  scroll_to(d, curvalue_ - page_incr_);
}
/*============================================================================*/
/* ColorValueReadout subclasses Observer, so that these objects 
 * can be attached to Adjustables (in our case, to ColorValues) and will
 * receive the update message from ColorValue::notify.  the
 * attachment takes place in the ColorValueReadout constructor.  when
 * the update message is received, the ColorValueReadout asks its
 * associated ColorValue for its current value, and then sprintf's that number 
 * into a string, and passes it to its  FieldEditor.
 */

class ColorValueReadout: public MonoGlyph, public Observer {
public:
  ColorValueReadout (ColorValue*, Style*);
  virtual ~ColorValueReadout ();
  virtual InputHandler* focusable () const;
  virtual void update (Observable*);
  virtual void disconnect (Observable*);
private:
  ColorValue* colorValue_;
  FieldEditor* editor_;
  void accept_editor (FieldEditor*);
  void cancel_editor (FieldEditor*);
};
/*----------------------------------------------------------------------------*/
/* These macros allow the user's typed numbers, in the text editor fields, to 
 * be passed back to theColorValueReadout.
 */

declareFieldEditorCallback(ColorValueReadout)
implementFieldEditorCallback(ColorValueReadout)
/*----------------------------------------------------------------------------*/
ColorValueReadout::ColorValueReadout 
      (ColorValue* colorValue, Style* style): MonoGlyph (nil) 
{
  Style* s = new Style (style);
  s->alias ("ColorValueReadout");
  colorValue_ = colorValue;
  colorValue->attach (Dimension_X, this);
  editor_ = DialogKit::instance()->field_editor (
	"100.00", s,
	new FieldEditorCallback(ColorValueReadout)(
            this, &ColorValueReadout::accept_editor, 
            &ColorValueReadout::cancel_editor
        )
    );
  body (editor_);
  update (colorValue->observable (Dimension_X));
}
/*----------------------------------------------------------------------------*/
ColorValueReadout::~ColorValueReadout () 
{
  if (colorValue_ != nil) 
     colorValue_->detach(Dimension_X, this);
}
/*----------------------------------------------------------------------------*/
InputHandler* ColorValueReadout::focusable () const 
{
  return editor_;
}
/*----------------------------------------------------------------------------*/
void ColorValueReadout::update (Observable*) 
{
  int scaledColorValue = (int) (255 * colorValue_->cur_lower (Dimension_X));
  char buf [20];
  sprintf (buf, "%2d", scaledColorValue);
  editor_->field (buf);
}
/*----------------------------------------------------------------------------*/
void ColorValueReadout::disconnect (Observable*) 
{
  colorValue_ = nil;
}
/*----------------------------------------------------------------------------*/
void ColorValueReadout::accept_editor (FieldEditor*) 
{
  Coord v;
  const String& value = *editor_->text();
  if (value.convert (v)) 
     colorValue_->current_value (v);
}
/*----------------------------------------------------------------------------*/
void ColorValueReadout::cancel_editor(FieldEditor*) 
{
  update (colorValue_->observable (Dimension_X));
}
/*============================================================================*/
/* ColorDisplay.  this class is attached to ColorValue objects, from
 * which they receive update objects whenever the ColorValues change.
 * they find out the current rgb values
 */
class ColorDisplay: public Patch, public Observer {
public:
    ColorDisplay (ColorValue*, ColorValue*, ColorValue*, Style*);
    virtual ~ColorDisplay ();

    virtual void allocate (Canvas*, const Allocation&, Extension&);
    virtual void draw (Canvas*, const Allocation&) const;
    virtual void update (Observable*);
    virtual void disconnect (ColorValue*);
private:
    Adjustable* red_;
    Adjustable* green_;
    Adjustable* blue_;
    Color* color_;
};
/*----------------------------------------------------------------------------*/
ColorDisplay::ColorDisplay (ColorValue* r, ColorValue* g, 
                            ColorValue* b, Style*) : Patch (nil) 
{
  red_ = r;
  red_->attach (Dimension_X, this);
  green_ = g;
  green_->attach (Dimension_X, this);
  blue_ = b;
  blue_->attach (Dimension_X, this);
  color_ = nil;
}
/*----------------------------------------------------------------------------*/
ColorDisplay::~ColorDisplay() 
{
  if (red_ != nil) 
     red_->detach(Dimension_X, this);

  if (green_ != nil) 
     green_->detach(Dimension_X, this);

    if (blue_ != nil) 
       blue_->detach(Dimension_X, this);
}
/*----------------------------------------------------------------------------*/
void ColorDisplay::allocate (Canvas* canvas, const 
                             Allocation& allocation, 
                             Extension& extension) 
{
    extension.set (canvas, allocation);
    Patch::allocate (canvas, allocation, extension);
}
/*----------------------------------------------------------------------------*/
void ColorDisplay::draw (Canvas* canvas, 
                         const Allocation& allocation) const 
{
  canvas->fill_rect (allocation.left (), allocation.bottom (), 
                     allocation.right (), allocation.top (), color_);
}
/*----------------------------------------------------------------------------*/
void ColorDisplay::update (Observable*) 
{
  Resource::unref (color_);
  color_ = new Color (
    float (red_->cur_lower(Dimension_X)),
    float (green_->cur_lower(Dimension_X)),
    float (blue_->cur_lower(Dimension_X)),1.0);

  Resource::ref (color_);
  redraw ();
}
/*----------------------------------------------------------------------------*/
void ColorDisplay::disconnect (ColorValue* a) 
{
  if (a == red_)
       red_ = nil;
  else if (a == green_) 
     green_ = nil;
  else if (a == blue_) 
    blue_ = nil;
}
/*============================================================================*/
/* DynamicLabel watches all three of the ColorValues, asks the RGBNames
 * class for a new string whenever any of the ColorValues change, and
 * then writes that new string to its patch.
 */
class DynamicLabel: public MonoGlyph, public Observer {
public:
  DynamicLabel::DynamicLabel (RGBNames*, ColorValue*, ColorValue*, 
                              ColorValue*, Style*);
  virtual void update ();
  virtual void update (Observable*);
private:
  Patch* patch_;
  Style* style_;
  char text_ [80];
  WidgetKit *kit;
  RGBNames *rgbLookupTable_;
  Adjustable* red_;
  Adjustable* green_;
  Adjustable* blue_;
};
/*----------------------------------------------------------------------------*/
DynamicLabel::DynamicLabel (RGBNames *lookupTable,
                            ColorValue *redValue, ColorValue *greenValue,
                            ColorValue *blueValue,  Style* style): 
  MonoGlyph (nil)
{
  style_ = style;
  kit = WidgetKit::instance ();
  patch_ = new Patch (nil);
  body (patch_);
  strcpy (text_, "no text yet");
  red_ = redValue;
  green_ = greenValue;
  blue_ = blueValue;
  rgbLookupTable_ = lookupTable;

  red_->attach (Dimension_X, this);
  green_->attach (Dimension_X, this);
  blue_->attach (Dimension_X, this);
  update ();
}
/*----------------------------------------------------------------------------*/
void DynamicLabel::update ()
{
  int scaledRed, scaledGreen, scaledBlue;

  scaledRed =   (int) (255 * red_->cur_lower (Dimension_X));
  scaledGreen = (int) (255 * green_->cur_lower (Dimension_X));
  scaledBlue =  (int) (255 * blue_->cur_lower (Dimension_X));
  sprintf (text_, "%s", 
           rgbLookupTable_->lookupName (scaledRed, scaledGreen, scaledBlue));

  patch_->body (new Label (text_, kit->font (), kit->foreground ()));
  patch_->redraw ();
  patch_->reallocate ();
  patch_->redraw ();

}
/*----------------------------------------------------------------------------*/
void DynamicLabel::update (Observable *)
{
  int scaledRed, scaledGreen, scaledBlue;

  scaledRed =   (int) (255 * red_->cur_lower (Dimension_X));
  scaledGreen = (int) (255 * green_->cur_lower (Dimension_X));
  scaledBlue =  (int) (255 * blue_->cur_lower (Dimension_X));
  sprintf (text_, "%s", 
           rgbLookupTable_->lookupName (scaledRed, scaledGreen, scaledBlue));

  patch_->body (new Label (text_, kit->font (), kit->foreground ()));
  patch_->redraw ();
  patch_->reallocate ();
  patch_->redraw ();

}
/*============================================================================*/
int main (int argc, char** argv) 
{
  Session* session = new Session ("ColorChooser", argc, argv);
  Style* style = session->style();
  WidgetKit& kit = *WidgetKit::instance();
  LayoutKit& layout = *LayoutKit::instance();
  ColorValue* redValue = new ColorValue (0.0, 1.0);
  ColorValue* greenValue = new ColorValue (0.0, 1.0);
  ColorValue* blueValue = new ColorValue (0.0, 1.0);
  RGBNames *rgbLookupTable;
  Glyph *colorVBox, *scrollersAndReadoutVBox, *mainHBox;
  ColorValueReadout *redReadOut,  *greenReadOut, *blueReadOut;

   /* construct the rgb name table, possibly using a path to a rgb.txt
    */
  if (argc == 2)
     rgbLookupTable = new RGBNames (argv [1]);
  else
     rgbLookupTable = new RGBNames ();

  DynamicLabel* colorNameDisplay = 
     new DynamicLabel (rgbLookupTable, redValue, greenValue, blueValue, style);
  Glyph *framedColorNameDisplay;

    /* InterViews requires us to use styles in order to set X resources
     * for glyphs.  the user interface has three scrollbars, side-by-side,
     * each topped off by a numerical readout.  we want the each of the 
     * three color readouts to use the appropriate foreground color, 
     * reminding the user which color the scrollbar manimpulates.
     * these three styles allow us to set the color of the text in the
     * readouts as X resources:
     *    ColorChooser*redReadOut*foreground: red
     *    ColorChooser*greenReadOut*foreground: green3
     *    ColorChooser*blueReadOut*foreground: blue
     */
  kit.begin_style ("redReadOut");
  redReadOut   = new ColorValueReadout (redValue, style);
  kit.end_style ();

  kit.begin_style ("greenReadOut");
  greenReadOut = new ColorValueReadout (greenValue, style);
  kit.end_style ();

  kit.begin_style ("blueReadOut");
  blueReadOut  = new ColorValueReadout (blueValue, style);
  kit.end_style ();

    /* create a bunch of layout glyphs, containing the user interface
     * objects, which are later assembled into the ApplicationWindow.
     * framedColorNameDisplay: this is an inset frame in which the
     *   name of the current color is displayed
     * colorVBox:  an unframed vertical box which contains the 
     *   framedColorNameDisplay, centered above an inset frame holding
     *   the color patch.
     * scrollersAndReadoutVBox: another vertical box, which has the
     *   three scrollbars topped off by the numerical readout boxes.
     * mainHBox: a horizontal box which assembles the colorVBox
     *   and the scrollersAndReadoutVbox side-by-side
     */



  framedColorNameDisplay = 
    kit.inset_frame (
      layout.hbox (
        layout.hglue (3,0,0),
        layout.h_fixed_span (
          colorNameDisplay, 
          155)
        )
      );

  colorVBox = 
     layout.vbox (
        layout.vglue (30,0,0),
        layout.hbox (
           layout.hglue (20,0,0),
           framedColorNameDisplay
           ),
        layout.vglue (10,0,0),
        kit.inset_frame (
          layout.fixed_span (
            new ColorDisplay (redValue, greenValue, blueValue, style),
            200.0, 200.0)
           )
        );

  scrollersAndReadoutVBox =
    layout.vbox (
      layout.vglue (63,0,0),  
      layout.hbox (
        layout.h_fixed_span (redReadOut, 25),
        layout.h_fixed_span (greenReadOut, 25),
        layout.h_fixed_span (blueReadOut, 25)),
        layout.v_fixed_span (
          layout.hbox ( 
            layout.h_fixed_span (kit.vscroll_bar (redValue), 25),
            layout.h_fixed_span (kit.vscroll_bar (greenValue), 25),
            layout.h_fixed_span (kit.vscroll_bar (blueValue), 25)
            ),
          185.0),
       layout.vglue (13,0,0)
     );
  
  mainHBox = 
      layout.hbox (
        layout.margin (
          colorVBox, 
          8.0),
        layout.margin (
          scrollersAndReadoutVBox, 
          8.0)
      );

  Window* w = new ApplicationWindow (
    new Background (
      kit.outset_frame (
        layout.hbox (
          layout.hglue (),
            layout.vbox (
              layout.vglue (),
              layout.margin (kit.push_button ("Quit", kit.quit()), 5.0),
              mainHBox,
              layout.vglue()
              ),
          layout.hglue ()
          )
        ),
      kit.background ()
      ) // Background
    ); // ApplicationWindow

  redValue->current_value (0.7);
  greenValue->current_value (0.6);
  blueValue->current_value (0.8);
  colorNameDisplay->update ();

  return session->run_window (w);

} // main
/*----------------------------------------------------------------------------*/
