#ifndef _TIMGFILTERSHARPEN_H_
#define _TIMGFILTERSHARPEN_H_

#include "TimgFilter.h"

struct TfilterSharpen;
class TimgFilterSharpen :public TimgFilter
{
private:
 unsigned char *Ymin,*Ymax; 
 void xsharpen(const unsigned char *src,unsigned char *dst,const TfilterSharpen *cfg);
 void unsharpen(const unsigned char *src,unsigned char *dst,const TfilterSharpen *cfg);
protected:
 virtual Trect* init(TffRect *rect,int full);
public:
 TimgFilterSharpen(void);
 virtual void done(void);
 virtual void process(TffPict2 &pict,const Tfilter *cfg0);
};

#endif
