#ifndef _TIMGFILTERCHROMA_H_
#define _TIMGFILTERCHROMA_H_

#include "TimgFilter.h"

class TimgFilterChroma :public TimgFilter
{
private:
 int hueSin[360+1],hueCos[360+1];
public:
 TimgFilterChroma(void);
 virtual void process(const unsigned char *srcY,const unsigned char *srcU,const unsigned char *srcV,
                      unsigned char *dstY,unsigned char *dstU,unsigned char *dstV,
                      const TpresetSettings *cfg);
};

#endif
