#ifndef _TIMGFILTERBLUR_H_
#define _TIMGFILTERBLUR_H_

#include "TimgFilter.h"

class TimgFilterBlur :public TimgFilter
{
private:
 __int64 kernel[3];
 int oldStrength;
 unsigned char *tempPict;
protected:
 virtual Trect* init(TffRect *rect,int full);
public:
 TimgFilterBlur(void);
 virtual void done(void);
 virtual void process(TffPict2 &pict,const TpresetSettings *cfg);
};

#endif
