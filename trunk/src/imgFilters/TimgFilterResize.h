#ifndef _TIMGFILTERRESIZE_H_
#define _TIMGFILTERRESIZE_H_

#include "TimgFilter.h"

struct SwsContext;
class TimgFilterResize :public TimgFilter
{
private:
 SwsContext *swsc;
 TffRect newRect;
 Trect calcNewClip(const TpresetSettings *cfg,const Trect &oldClip,const Trect &newFull);
public:
 TimgFilterResize(void);
 virtual void done(void);
 virtual void process(TffPict2 &pict,const TpresetSettings *cfg);
};

#endif
