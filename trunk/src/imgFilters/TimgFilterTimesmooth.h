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
 void smooth(const unsigned char *src,unsigned char *dst,unsigned char *Iaccum,unsigned int dx,unsigned int stride,unsigned int dy);
protected:
 virtual Trect* init(TffRect *rect,int full);
public:
 TimgFilterTimesmooth(void);
 virtual void done(void);
 virtual void process(TffPict *pict,TffRect &rect,const TpresetSettings *cfg);
};

#endif
