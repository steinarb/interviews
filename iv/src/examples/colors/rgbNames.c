/* rgbNames.c: a class that reads and stores the X11 color names
 * color names, stored in $OPENWINHOME/lib/rgb.txt, and which will return
 *    a color name which is a reasonable match for an rgb triplet.  for example
 *
 *        186  85 211   will return    MediumOrchid
 * 
 *    the numbers are from a scale that runs from 0 to 255.
 *
 * the RGBNames class is defined in this file, and may be tested here
 * as well, by defining EMBEDDED_TEST at compile time.  the class
 * is designed to be part of a simple Interviews exercise, in which
 * the user may interactively choose colors for display.  this class
 * allows the program to convert the rgb numerical triplet into a name
 * meaningful to the xnews server.
 *
 * the header file, rgbNames.h, declares the public portion of this class,
 * and should be included by any application that wants to use the class.
 */
/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
/*----------------------------------------------------------------------------*/
int extractRGBValuesAndColorName (char *rawString, 
                                  int *red, int *green, int *blue, 
                                  char *colorName);
/*----------------------------------------------------------------------------*/
const char *const RGB_FILE = "/usr/openwin/lib/rgb.txt";
const int MAX_COLORS = 800;
const int BIG_STRING_SIZE = 255;

class RGBNames {
public:
  RGBNames ();  
  RGBNames (const char *filename);
  char *lookupName (int red, int green, int blue);
private:
  struct rgbNameTableType {
     int red, green, blue;
     char name [40];
     };
  rgbNameTableType rgbNameTable [MAX_COLORS];
  int rgbtxtFileIsBeingUsed;
  int tableSize;
  int tolerance;   /* in matching an incoming rgb against a table entry */
  void init (const char *filename);
};
/*----------------------------------------------------------------------------*/
void RGBNames::init (const char *filename)
{
  FILE *fp;
  char rawString [BIG_STRING_SIZE];
  int red, green, blue;
  char colorName [40];
  int  badLines = 0;
  int totalLines = 0;
  
  if ((fp = fopen (filename, "r")) == (FILE *) NULL) {
     rgbtxtFileIsBeingUsed = 0;
     return;
     } /* fopen failed */

    /* we found the file, so read it into the table.
     * todo:  check the file for integrity
     */

  rgbtxtFileIsBeingUsed = 1;

  tableSize = 0;
  while (fgets (rawString, BIG_STRING_SIZE - 1, fp) != (char *) NULL) {
     totalLines++;
     rawString [strlen (rawString) - 1] = 0; /* strip linefeed */
     if (extractRGBValuesAndColorName (rawString, &red, &green, &blue, 
                                       colorName)) {
        rgbNameTable [tableSize].red = red;
        rgbNameTable [tableSize].green = green;
        rgbNameTable [tableSize].blue = blue;
        strcpy (rgbNameTable [tableSize].name, colorName);
        tableSize++;
        } /* if good extraction -- no spaces in the color name */
     else
        badLines++;
     } /* while not eof */

  tolerance = 10;

  fclose (fp);

} /* RGBNames::init */
/*----------------------------------------------------------------------------*/
RGBNames::RGBNames ()
{
  rgbtxtFileIsBeingUsed = 0;

} /* RGBNames constructor */
/*----------------------------------------------------------------------------*/
RGBNames::RGBNames (const char *filename)
{
 init (filename);

} /* RGBNames constructor */
/*----------------------------------------------------------------------------*/
char *RGBNames::lookupName (int red, int green, int blue)
/* return the name of the color with rgb values closest to those supplied
 * in the parameter list.  "closest" is the table entry with the smallest
 * error:    abs (redRequested   - redInTable [i])  +
 *           abs (greenRequested - greenInTable [i])  +
 *           abs (blueRequested  - blueInTable [i])  +
 * this definition, and its implementation are hasty, brute force
 * techniques.  lots of improvement is possible...
 */
{
  int exactlyMatchingEntry, exactMatch = 0;
  int redError, greenError, blueError;
  char hexColorName [20];
  char temp [20];

  if (rgbtxtFileIsBeingUsed) {
     for (int i=0; i < tableSize; i++) {
        redError = abs (red - rgbNameTable [i].red);
        greenError = abs (green - rgbNameTable [i].green);
        blueError = abs (blue - rgbNameTable [i].blue);
        if (redError + greenError + blueError < tolerance) {
           exactMatch = 1;
           exactlyMatchingEntry = i;
           break;
           } /* if exact match */
        } /* for i */
     if (exactMatch)
        return rgbNameTable [exactlyMatchingEntry].name;
     } /* if rgbtxtFileIsBeingUsed */

   /* either there's no rgb.txt file in use, or no exact match was found.
    * so cobble together the hex value of the current rgb triplet:
    */

  sprintf (temp, "%02x%02x%02x", red, green, blue);
  strcpy (hexColorName, "#");
  strcat (hexColorName, temp);
  return hexColorName;

} /* RGBNames:: lookupName */
/*----------------------------------------------------------------------------*/
int extractRGBValuesAndColorName (char *rawString, 
                                  int *red, int *green, int *blue, 
                                  char *colorName)
/* X11 stores colors like this:
 * 186  85 211             medium orchid
 * 186  85 211             MediumOrchid
 * here, we're only interested in the second form, in which the color
 * name is one continous word, with now embedded blanks.  extract the tokens
 * from <rawString>, and figure out if there are blanks in the color name (by
 * trying to extract a 5th token).  return the values via the function 
 * arguments, and return the function's value
 *    1: color name is only one word
 *    0: more than one word
 */

{
  static char  *separators = " \t";  /* space and tab */
  char   *newToken;
  int    colorNameIsOnlyOneWord;
  static int timesCalled = 0;

  newToken = strtok (rawString, separators);
  assert (newToken);
  *red = atoi (newToken);

  newToken = strtok ((char *) NULL, separators);
  assert (newToken);
  *green = atoi (newToken);

  newToken = strtok ((char *) NULL, separators);
  assert (newToken);
  *blue = atoi (newToken);

  newToken = strtok ((char *) NULL, separators);
  assert (newToken);
  strcpy (colorName, newToken);

  if (strtok ((char *) NULL, separators))
     colorNameIsOnlyOneWord = 0;
  else
     colorNameIsOnlyOneWord = 1;
 
  return colorNameIsOnlyOneWord;

} /* extract rgb values and color names */
/*----------------------------------------------------------------------------*/
#ifdef EMBEDDED_TEST
/*------------------*/
int main (int, char **)
{
  int red, green, blue;
  char colorName [80];

  RGBNames rgbLookupTable ("/usr/openwin/lib/rgb.txt");

  while (1) {
     printf ("enter rgb value (red green blue)\n");
     scanf ("%d %d %d", &red, &green, &blue);

     strcpy (colorName, rgbLookupTable.lookupName (red, green, blue));
     printf ("best match color name for (%d, %d, %d): %s\n", 
             red, green, blue, colorName);

     } /* while */

  return 0;

} /* main */
/*----------------------------------------------------------------------------*/
#endif
