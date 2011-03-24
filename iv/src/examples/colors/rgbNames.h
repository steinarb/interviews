class RGBNames {
public:
  RGBNames ();  
  RGBNames (const char *filename);
  char *lookupName (int red, int green, int blue);
  void lookupTriplet (int *red, int *green, int *blue);
};
