#ifndef _TIMGFILTERSHARPEN_H_
#define _TIMGFILTERSHARPEN_H_

#include "TimgFilter.h"

class TimgFilterSharpen :public TimgFilter
{
private:
 unsigned char *Ymin,*Ymax; 
 void xsharpen(unsigned char *src,unsigned char *dst,Tconfig *cfg);
 void unsharpen(unsigned char *src,unsigned char *dst,Tconfig *cfg);
protected:
 virtual void done(void);
public:
 TimgFilterSharpen(void);
 virtual void init(int Idx,int Istride,int Idy);
 virtual void process(unsigned char *srcY,unsigned char *srcU,unsigned char *srcV,
                      unsigned char *dstY,unsigned char *dstU,unsigned char *dstV,
                      Tconfig *cfg);
};

#endif
