#ifndef _TIMGFILTERSHOWMV_H_
#define _TIMGFILTERSHOWMV_H_

#include "TimgFilter.h"

class TimgFilterShowMV :public TimgFilter
{
private:
 int mv_dx,mv_dy;
 char *mv;
 void line(unsigned char *dst,int x0,int y0,int x1,int y1);
public:
 bool firsttime;
 virtual void init(int Idx,int Istride,int Idy);
 void setMV(int Imv_dx,int Imv_dy,char *Imv);
 virtual void process(const unsigned char *srcY,const unsigned char *srcU,const unsigned char *srcV,
                      unsigned char *dstY,unsigned char *dstU,unsigned char *dstV,
                      const TpresetSettings *cfg);
};

#endif
