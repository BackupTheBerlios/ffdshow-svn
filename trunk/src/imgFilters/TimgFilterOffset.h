#ifndef _TIMGFILTEROFFSET_H_
#define _TIMGFILTEROFFSET_H_

#include "TimgFilter.h"

class TimgFilterOffset :public TimgFilter
{
private:
 int old_offsetY_X,old_offsetY_Y,old_offsetU_X,old_offsetU_Y,old_offsetV_X,old_offsetV_Y;
 int oldOrder;
 void offset(const unsigned char *src,unsigned char *dst,int dx,int stride,int dy,int offsetX,int offsetY);
public:
 TimgFilterOffset(void);
 virtual void init(int Idx,int Istride,int Idy);
 virtual void process(TtempPictures *pict,const TpresetSettings *cfg);
};

#endif
