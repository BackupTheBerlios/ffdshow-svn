#ifndef _TIMGFILTERLUMA_H_
#define _TIMGFILTERLUMA_H_

#include "TimgFilter.h"

class TimgFilterLuma :public TimgFilter
{
private:
 unsigned int gammaTab[256];int oldGamma;
 void processGainOffset(const unsigned char *srcY,unsigned char *dstY,const TpresetSettings *cfg);
 void processGamma(const unsigned char *srcY,unsigned char *dstY,const TpresetSettings *cfg);
public:
 TimgFilterLuma(void);
 virtual void process(TffPict2 &pict,const TpresetSettings *cfg);
};

#endif
