#ifndef _TIMGFILTERTIMESMOOTH_H_
#define _TIMGFILTERTIMESMOOTH_H_

#include "TimgFilter.h"

class TimgFilterTimesmooth :public TimgFilter
{
private:
 int oldStrength;
 unsigned char *accumY,*accumU,*accumV;
 int framecount;
 int square_table[255*2+1];
 void smooth(const unsigned char *src,unsigned char *dst,unsigned char *Iaccum,int dx,int stride,int dy);
protected:
 virtual void done(void);
public:
 TimgFilterTimesmooth(void);
 virtual void init(int Idx,int Istride,int Idy);
 virtual void process(const unsigned char *srcY,const unsigned char *srcU,const unsigned char *srcV,
                      unsigned char *dstY,unsigned char *dstU,unsigned char *dstV,
                      const TpresetSettings *cfg);
};

#endif
