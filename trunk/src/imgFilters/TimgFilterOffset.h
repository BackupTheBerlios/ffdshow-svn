#ifndef _TIMGFILTEROFFSET_H_
#define _TIMGFILTEROFFSET_H_

#include "TimgFilter.h"

class TimgFilterOffset :public TimgFilter
{
private:
 void offset(const unsigned char *src,unsigned char *dst,unsigned int dx,unsigned int stride,unsigned int dy,unsigned int offsetX,unsigned int offsetY,unsigned char c);
public:
 virtual void process(TffPict *pict,TffRect &rect,const TpresetSettings *cfg);
};

#endif
