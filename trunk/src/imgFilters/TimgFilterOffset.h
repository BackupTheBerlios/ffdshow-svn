#ifndef _TIMGFILTEROFFSET_H_
#define _TIMGFILTEROFFSET_H_

#include "TimgFilter.h"

class TimgFilterOffset :public TimgFilter
{
private:
 void offset(const unsigned char *src,unsigned char *dst,int dx,int stride,int dy,int offsetX,int offsetY,int c);
public:
 virtual void init(int Idx,int Istride,int Idy);
 virtual void process(TtempPictures *pict,const TpresetSettings *cfg);
};

#endif
