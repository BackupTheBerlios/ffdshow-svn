#ifndef _TIMGFILTEROFFSET_H_
#define _TIMGFILTEROFFSET_H_

#include "TimgFilter.h"

class TimgFilterOffset :public TimgFilter
{
private:
 int old_offsetY_X,old_offsetY_Y,old_offsetU_X,old_offsetU_Y,old_offsetV_X,old_offsetV_Y;
 void offset(const unsigned char *src,unsigned char *dst,int dx,int stride,int dy,int offsetX,int offsetY);
public:
 TimgFilterOffset(void);
 virtual void init(int Idx,int Istride,int Idy);
 virtual void process(const unsigned char *srcY,const unsigned char *srcU,const unsigned char *srcV,
                      unsigned char *dstY,unsigned char *dstU,unsigned char *dstV,
                      const TpresetSettings *cfg);
};

#endif
