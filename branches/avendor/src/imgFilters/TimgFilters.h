#ifndef _TIMGFILTERS_H_
#define _TIMGFILTERS_H_

#include <stdlib.h>
#include "..\TcpuUsage.h"
#include "..\Tpostproc.h"
#include "TtempPicture.h"
#include "TimgFilterNoise.h"
#include "TimgFilterLuma.h"
#include "TimgFilterChroma.h"
#include "TimgFilterSharpen.h"
#include "TimgFilterSubtitles.h"

class Tconfig;
class TimgFilters
{
private:
 Tconfig *cfg;
 int dxY ,diffY ,strideY;
 int dxUV,diffUV,strideUV;
 int dy;
 int _dx,_dy;
 TcpuUsage cpu;int cpus;
 TtempPicture *tempY,*tempU,*tempV;
 TimgFilterNoise noise;
 TimgFilterLuma luma;
 TimgFilterChroma chroma;
 TimgFilterSharpen sharpen;
 TimgFilterSubtitles subtitles;
public:
 TimgFilters(Tconfig *Icfg);
 ~TimgFilters();
 void init(int IdxY,int IstrideY,int Idy,int IdiffX,int IdiffY);
 void done(void);
 void process(unsigned char *srcY,unsigned char *srcU,unsigned char *srcV,
              unsigned char**dstY,unsigned char**dstU,unsigned char**dstV,
              int *quant_store);
 Tpostproc postproc;
};

#endif
