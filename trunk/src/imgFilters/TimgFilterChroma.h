#ifndef _TIMGFILTERCHROMA_H_
#define _TIMGFILTERCHROMA_H_

#include "TimgFilter.h"

class TimgFilterChroma :public TimgFilter
{
private:
 int hueSin[360+1],hueCos[360+1];
public:
 TimgFilterChroma(void);
// void hueSat(unsigned char *srcU,unsigned char *srcV,unsigned char *dstU,unsigned char *dstV,Tconfig *cfg);
 virtual void process(unsigned char *srcY,unsigned char *srcU,unsigned char *srcV,
                      unsigned char *dstY,unsigned char *dstU,unsigned char *dstV,
                      Tconfig *cfg);
};

#endif
