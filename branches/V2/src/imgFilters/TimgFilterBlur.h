#ifndef _TIMGFILTERBLUR_H_
#define _TIMGFILTERBLUR_H_

#include "TimgFilter.h"

struct TfilterBlur;
class TimgFilterBlur :public TimgFilter
{
private:
 //--- blur ---
 __int64 kernel[3];
 int oldBlurStrength;
 unsigned char *blurTempPict;
 void processBlur(const unsigned char *srcY,unsigned char *dstY,const TfilterBlur *cfg);
 //--- timesmooth ---
 int oldTimesmoothStrength;
 unsigned char *accumY,*accumU,*accumV;
 int framecount;
 int square_table[255*2+1];
 void timesmooth(const unsigned char *src,unsigned char *dst,unsigned char *Iaccum,unsigned int dx,unsigned int stride,unsigned int dy);
 void processTimesmooth(const unsigned char *srcY,const unsigned char *srcU,const unsigned char *srcV,unsigned char *dstY,unsigned char *dstU,unsigned char *dstV,const TfilterBlur *cfg);
public:
 TimgFilterBlur(void);
 virtual void done(void);
 virtual void process(TffPict2 &pict,const Tfilter *cfg0);
};

#endif
