#ifndef _TIMGFILTERBLUR_H_
#define _TIMGFILTERBLUR_H_

#include "TimgFilter.h"

class TimgFilterBlur :public TimgFilter
{
private:
 __int64 kernel[3];
 int oldStrength;
 unsigned char *tempPict;
protected:
 virtual void done(void);
public:
 TimgFilterBlur(void);
 virtual void init(int Idx,int Istride,int Idy);
 virtual void process(const unsigned char *srcY,const unsigned char *srcU,const unsigned char *srcV,
                      unsigned char *dstY,unsigned char *dstU,unsigned char *dstV,
                      const TpresetSettings *cfg);
};

#endif
