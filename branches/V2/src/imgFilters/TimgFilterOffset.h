#ifndef _TIMGFILTEROFFSET_H_
#define _TIMGFILTEROFFSET_H_

#include "TimgFilter.h"

class TimgFilterOffset :public TimgFilter
{
private:
 void offset(const unsigned char *src,unsigned char *dst,unsigned int dx,unsigned int stride,unsigned int dy,int offsetX,int offsetY,unsigned char c);
public:
 virtual void process(TffPict2 &pict,const TpresetSettings *cfg);
};

#endif
