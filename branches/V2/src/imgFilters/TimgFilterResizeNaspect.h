#ifndef _TIMGFILTERRESIZENASPECT_H_
#define _TIMGFILTERRESIZENASPECT_H_

#include "TimgFilter.h"
#include "TfilterResizeNaspect.h"

struct SwsContext;
class TimgFilterResizeNaspect :public TimgFilter
{
private:
 TfilterResizeNaspect::Tsettings oldSettings;
 SwsContext *swsc;
 TffRect newRect;
 Trect calcNewClip(const TfilterResizeNaspect *cfg,const Trect &oldClip,const Trect &newFull);
public:
 TimgFilterResizeNaspect(void);
 virtual void done(void);
 virtual void process(TffPict2 &pict,const Tfilter *cfg0);
};

#endif
