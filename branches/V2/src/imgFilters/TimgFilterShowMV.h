#ifndef _TIMGFILTERSHOWMV_H_
#define _TIMGFILTERSHOWMV_H_

#include "TimgFilter.h"

class TmovieSource;
class TimgFilterShowMV :public TimgFilter
{
private:
 void line(unsigned char *dst,unsigned int x0,unsigned int y0,unsigned int x1,unsigned int y1);
public:
 virtual void process(TffPict2 &pict,const Tfilter *cfg0);
};

#endif
