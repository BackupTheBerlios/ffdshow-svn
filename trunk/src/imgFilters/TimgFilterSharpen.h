#ifndef _TIMGFILTERSHARPEN_H_
#define _TIMGFILTERSHARPEN_H_

#include "TimgFilter.h"

class TimgFilterSharpen :public TimgFilter
{
private:
 unsigned char *Ymin,*Ymax; 
 void xsharpen(const unsigned char *src,unsigned char *dst,const TpresetSettings *cfg);
 void unsharpen(const unsigned char *src,unsigned char *dst,const TpresetSettings *cfg);
protected:
 virtual TffRect::Trect* init(TffRect *rect,int full);
 virtual void done(void);
public:
 TimgFilterSharpen(void);
 virtual void process(TtempPictures *pict,TffRect &rect,const TpresetSettings *cfg);
};

#endif
