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
 virtual TffRect::Trect* init(TffRect *rect,int full);
 virtual void done(void);
public:
 TimgFilterBlur(void);
 virtual void process(TtempPictures *pict,TffRect &rect,const TpresetSettings *cfg);
};

#endif
