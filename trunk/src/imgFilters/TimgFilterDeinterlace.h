#ifndef _TIMGFILTERDEINTERLACE_H_
#define _TIMGFILTERDEINTERLACE_H_

#include "TimgFilter.h"

class TimgFilterDeinterlace :public TimgFilter
{
private:
 static const int deintMethods[];
public:
 virtual void process(TffPict2 &pict,const TpresetSettings *cfg);
};

#endif
