#ifndef _TIMGFILTERSHOWMV_H_
#define _TIMGFILTERSHOWMV_H_

#include "TimgFilter.h"

class TmovieSource;
class TimgFilterShowMV :public TimgFilter
{
private:
 void line(unsigned char *dst,int x0,int y0,int x1,int y1);
public:
 bool firsttime;
 virtual void init(int Idx,int Istride,int Idy);
 virtual void process(TtempPictures *pict,const TpresetSettings *cfg) {};
 void process(TtempPictures *pict,const TpresetSettings *cfg,const TmovieSource *movie);
};

#endif
