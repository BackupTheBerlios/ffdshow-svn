#ifndef _TPRESETSETTINGS_H_
#define _TPRESETSETTINGS_H_

#include <vector>
#include "IffConstants.h"

struct Tfilter;
struct TregOp;
struct IffDecoder;
struct TpresetSettings
{
private:
 void reg_op(TregOp &t);
 void initFilters(IffDecoder *deci),doneFilters(void);
public:
 TpresetSettings(IffDecoder *dec,const char *IpresetName=FFPRESET_DEFAULT);
 TpresetSettings(TpresetSettings&);
 ~TpresetSettings();
 void loadDefault(void);
 void loadReg(void),saveReg(void);
 void loadFile(const char *flnm),saveFile(const char *flnm);
 std::vector<Tfilter*> filters;
 Tfilter* getFilter(int filterID);
 void sortOrder(void);

 struct Tsettings
  { 
   int flip;
   int idct;
   char presetName[260];
   int autoLoadedFromFile;
  } settings; 
 
 static void normalizePresetName(char *dst,const char *src)
  {
   char c;
   do
   {
    c=*(src++);
    if (c=='\\') c='/';
    *(dst++)=c;
   } while (c);
  }
 static bool isValidPresetName(const char *presetName);
};

#endif
