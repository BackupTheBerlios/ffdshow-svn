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
 virtual void done(void);
public:
 TimgFilterSharpen(void);
 virtual void init(int Idx,int Istride,int Idy);
 virtual void process(TtempPictures *pict,const TpresetSettings *cfg);
};

#endif
