#ifndef _TIMGFILTERLUMA_H_
#define _TIMGFILTERLUMA_H_

#include "TimgFilter.h"

class TimgFilterLuma :public TimgFilter
{
private:
 unsigned int gammaTab[256];int oldGamma;
public:
 TimgFilterLuma(void);
 virtual void process(TffPict2 &pict,const TpresetSettings *cfg);
};

#endif
