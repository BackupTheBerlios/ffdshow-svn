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
 virtual void done(void);
public:
 TimgFilterBlur(void);
 virtual void init(int Idx,int Istride,int Idy);
 virtual void process(TtempPictures *pict,const TpresetSettings *cfg);
};

#endif
