#ifndef _TIMGFILTERPICTPROP_H_
#define _TIMGFILTERPICTPROP_H_

#include "TimgFilter.h"

struct TfilterPictProp;
class TimgFilterPictProp :public TimgFilter
{
private:
 unsigned int gammaTab[256];int oldGamma;
 int hueSin[360+1],hueCos[360+1];
 void processLuma  (const unsigned char *srcY,unsigned char *dstY,const TfilterPictProp *cfg);
 void processGamma (const unsigned char *srcY,unsigned char *dstY,const TfilterPictProp *cfg);
 void processChroma(const unsigned char *srcU,const unsigned char *srcV,unsigned char *dstU,unsigned char *dstV,const TfilterPictProp *cfg);
public:
 TimgFilterPictProp(void);
 virtual void process(TffPict2 &pict,const Tfilter *cfg0);
};

#endif
