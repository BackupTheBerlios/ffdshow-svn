#ifndef _TIMGFILTERSHOWMV_H_
#define _TIMGFILTERSHOWMV_H_

#include "TimgFilter.h"

class TmovieSource;
class TimgFilterShowMV :public TimgFilter
{
private:
 void line(unsigned char *dst,int x0,int y0,int x1,int y1);
public:
 virtual void process(TtempPictures *pict,TffRect &rect,const TpresetSettings *cfg);
};

#endif
