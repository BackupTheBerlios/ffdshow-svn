#ifndef _TIMGFILTEROFFSET_H_
#define _TIMGFILTEROFFSET_H_

#include "TimgFilter.h"

class TimgFilterOffset :public TimgFilter
{
private:
 void offset(const unsigned char *src,unsigned char *dst,int dx,int stride,int dy,int offsetX,int offsetY,int c);
public:
 virtual void process(TtempPictures *pict,TffRect &rect,const TpresetSettings *cfg);
};

#endif
