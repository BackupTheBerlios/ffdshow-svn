#ifndef _TIMGFILTERCROP_H_
#define _TIMGFILTERCROP_H_

#include "TimgFilter.h"

class TimgFilterCrop :public TimgFilter
{
private:
 unsigned int cropLeft,cropTop,cropDx,cropDy;
 TffRect oldRect;
 void calcCrop(const Trect *r,const TpresetSettings *cfg);
public:
 TimgFilterCrop(void);
 virtual void done(void);
 virtual void process(TffPict *pict,TffRect &rect,const TpresetSettings *cfg);
};

#endif
