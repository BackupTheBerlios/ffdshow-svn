#ifndef _TIMGFILTEROFFSET_H_
#define _TIMGFILTEROFFSET_H_

#include "TimgFilter.h"

class TimgFilterOffset :public TimgFilter
{
public:
 TimgFilterOffset(void);
 virtual void init(int Idx,int Istride,int Idy);
 virtual void process(unsigned char *srcY,unsigned char *srcU,unsigned char *srcV,
                      unsigned char *dstY,unsigned char *dstU,unsigned char *dstV,
                      TpresetSettings *cfg);
};

#endif
