#ifndef _TIMGFILTERRESIZE_H_
#define _TIMGFILTERRESIZE_H_

#include "TimgFilter.h"

struct SwsContext;
class TimgFilterResize :public TimgFilter
{
private:
 SwsContext *swsc;
 TffRect newRect;
 int resizeMethodOld,resizeGblurLumOld,resizeGblurChromOld,resizeSharpenLumOld,resizeSharpenChromOld;
 Trect calcNewClip(const TpresetSettings *cfg,const Trect &oldClip,const Trect &newFull);
public:
 TimgFilterResize(void);
 virtual void done(void);
 virtual void process(TffPict *pict,TffRect &rect,const TpresetSettings *cfg);
};

#endif
