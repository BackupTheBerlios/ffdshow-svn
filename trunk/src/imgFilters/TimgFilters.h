#ifndef _TIMGFILTERS_H_
#define _TIMGFILTERS_H_

#include <stdlib.h>
#include <vector>
#include "TimgFilterPostproc.h"
#include "TimgFilterNoise.h"
#include "TimgFilterLuma.h"
#include "TimgFilterChroma.h"
#include "TimgFilterSharpen.h"
#include "TimgFilterSubtitles.h"
#include "TimgFilterBlur.h"
#include "TimgFilterOffset.h"
#include "TimgFilterTimesmooth.h"
#include "TimgFilterShowMV.h"

struct TglobalSettings;
struct TpresetSettings;
struct subtitle;
struct TtempPictures;
struct IffDecoder;
class TimgFilters
{
private:
 int dxY ,strideY;
 int dxUV,strideUV;
 int dy;
 TtempPictures *tempPict;
 IffDecoder *deci;
 std::vector<TimgFilter*> filters;
 TimgFilterPostproc postproc;
 TimgFilterNoise noise;
 TimgFilterLuma luma;
 TimgFilterChroma chroma;
 TimgFilterBlur blur;
 TimgFilterSharpen sharpen;
 TimgFilterSubtitles subtitles;
 TimgFilterOffset offset;
 TimgFilterTimesmooth timesmooth;
 TimgFilterShowMV showMV;
public:
 TimgFilters(IffDecoder *Ideci):deci(Ideci),tempPict(NULL) {};
 ~TimgFilters();
 void init(int IdxY,int IstrideY,int Idy,int dyFull,int IdiffX,int IdiffY);
 void done(void);
 void setSubtitle(subtitle *Isub);
 void process(const TglobalSettings *global,const TpresetSettings *cfg,
              unsigned char *srcY,unsigned char *srcU,unsigned char *srcV,
              unsigned char**dstY,unsigned char**dstU,unsigned char**dstV);
};

#endif
