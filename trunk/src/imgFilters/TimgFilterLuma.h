#ifndef _TIMGFILTERLUMA_H_
#define _TIMGFILTERLUMA_H_

#include "TimgFilter.h"

class TimgFilterLuma :public TimgFilter
{
private:
 unsigned char gammaTab[256];int oldGamma;
 __int64 c1,c2,c3;
 void calc123(int g0);
public:
 TimgFilterLuma(void);
 virtual void process(unsigned char *srcY,unsigned char *srcU,unsigned char *srcV,
                      unsigned char *dstY,unsigned char *dstU,unsigned char *dstV,
                      TpresetSettings *cfg);
};

#endif
