#ifndef _TIMGFILTERBLUR_H_
#define _TIMGFILTERBLUR_H_

#include "TimgFilter.h"

class TimgFilterBlur :public TimgFilter
{
private:
 int kernel[5];
 int oldStrength;
public:
 TimgFilterBlur(void);
 virtual void process(unsigned char *srcY,unsigned char *srcU,unsigned char *srcV,
                      unsigned char *dstY,unsigned char *dstU,unsigned char *dstV,
                      Tconfig *cfg);
};

#endif
