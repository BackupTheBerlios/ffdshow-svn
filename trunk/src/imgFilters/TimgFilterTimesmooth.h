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
 virtual TffRect::Trect* init(TffRect *rect,int full);
 virtual void done(void);
public:
 TimgFilterTimesmooth(void);
 virtual void process(TtempPictures *pict,TffRect &rect,const TpresetSettings *cfg);
};

#endif
