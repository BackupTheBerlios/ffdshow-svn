#ifndef _TIMGFILTERCROP_H_
#define _TIMGFILTERCROP_H_

#include "TimgFilter.h"
#include "TpresetSettings.h"

class TimgFilterCrop :public TimgFilter
{
private:
 int cropLeft,cropTop,cropDx,cropDy;
 void calcCrop(const Trect *r,const TpresetSettings *cfg);
 TpresetSettings::TcropSettings oldSettings;
public:
 TimgFilterCrop(void);
 virtual void done(void);
 virtual void process(TffPict2 &pict,const TpresetSettings *cfg);
};

#endif
