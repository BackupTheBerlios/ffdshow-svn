#ifndef _TIMGFILTERLUMA_H_
#define _TIMGFILTERLUMA_H_

#include "TimgFilter.h"

class TimgFilterLuma :public TimgFilter
{
private:
 unsigned int gammaTab[256];int oldGamma;
public:
 TimgFilterLuma(void);
 virtual void process(TtempPictures *pict,TffRect &rect,const TpresetSettings *cfg);
};

#endif
