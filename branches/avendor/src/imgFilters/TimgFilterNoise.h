#ifndef _TIMGFILTERNOISE_H_
#define _TIMGFILTERNOISE_H_

#include "TimgFilter.h"

class TimgFilterNoise :public TimgFilter
{
private:
 int noiseAvihStrenth,noiseAvihStrenthChroma;
 short *noiseMaskY;int noiseCountY;
 short *noiseMaskU;int noiseCountU;
 short *noiseMaskV;int noiseCountV;
 void noise0luma(unsigned char *src,unsigned char *dst,int stride,int dx,int dy,int noiseStrength,int uniformNoise,short *noiseMask,int noiseCount);
 void noise0chroma(unsigned char *src,unsigned char *dst,int stride,int dx,int dy,int noiseStrength,int uniformNoise,short *noiseMask,int noiseCount);
 void noiseY     (unsigned char *src ,unsigned char *dst ,Tconfig *cfg); 
 void noiseUV    (unsigned char *srcU,unsigned char *dstU,unsigned char *srcV,unsigned char *dstV,Tconfig *cfg);
 void noiseAvihY (unsigned char *src ,unsigned char *dst ,Tconfig *cfg); 
 void noiseAvihUV(unsigned char *srcU,unsigned char *dstU,unsigned char *srcV,unsigned char *dstV,Tconfig *cfg);
protected:
 virtual void done(void);
public:
 TimgFilterNoise(void);
 void init(int Idx,int Istride,int Idy);
 virtual void process(unsigned char *srcY,unsigned char *srcU,unsigned char *srcV,
                      unsigned char *dstY,unsigned char *dstU,unsigned char *dstV,
                      Tconfig *cfg);
};

#endif
