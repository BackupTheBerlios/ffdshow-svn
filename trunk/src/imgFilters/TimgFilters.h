#ifndef _TIMGFILTERS_H_
#define _TIMGFILTERS_H_

#include <stdlib.h>
#include <vector>
#include "TcpuUsage.h"
#include "Tpostproc.h"
#include "TtempPicture.h"
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
class TmovieSource;
struct subtitle;
class TimgFilters
{
private:
 int dxY ,diffY ,strideY;
 int dxUV,diffUV,strideUV;
 int dy;
 //int _dx,_dy;
 bool afterResize;
 TcpuUsage cpu;int cpus;
 TtempPicture *tempY,*tempU,*tempV;
 std::vector<TimgFilter*> filters;
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
 TimgFilters(void);
 ~TimgFilters();
 void init(int IdxY,int IstrideY,int Idy,int dyFull,int IdiffX,int IdiffY,bool IafterResize);
 void done(void);
 void setSubtitle(subtitle *Isub);
 void process(TglobalSettings *global,TpresetSettings *cfg,TmovieSource *movie,
              unsigned char *srcY,unsigned char *srcU,unsigned char *srcV,
              unsigned char**dstY,unsigned char**dstU,unsigned char**dstV);
 Tpostproc postproc;
};

#endif
