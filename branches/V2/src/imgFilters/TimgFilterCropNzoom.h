#ifndef _TIMGFILTERCROPNZOOM_H_
#define _TIMGFILTERCROPNZOOM_H_

#include "TimgFilter.h"
#include "TfilterCropNzoom.h"

class TimgFilterCropNzoom :public TimgFilter
{
private:
 TfilterCropNzoom::Tsettings settingsOld;
 int cropLeft,cropTop,cropDx,cropDy;
 void calcCrop(const Trect *r,const TfilterCropNzoom *cfg);
public:
 TimgFilterCropNzoom(void);
 virtual void done(void);
 virtual void process(TffPict2 &pict,const Tfilter *cfg0);
};

#endif
