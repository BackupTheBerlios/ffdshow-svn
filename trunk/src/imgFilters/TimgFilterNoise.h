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
 void noise0luma  (const unsigned char *src ,unsigned char *dst ,int stride,int dx,int dy,int noiseStrength,int uniformNoise,short *noiseMask,int noiseCount);
 void noise0chroma(const unsigned char *src ,unsigned char *dst ,int stride,int dx,int dy,int noiseStrength,int uniformNoise,short *noiseMask,int noiseCount);
 void noiseY      (const unsigned char *src ,unsigned char *dst ,const TpresetSettings *cfg); 
 void noiseUV     (const unsigned char *srcU,unsigned char *dstU,const unsigned char *srcV,unsigned char *dstV,const TpresetSettings *cfg);
 void noiseAvihY  (const unsigned char *src ,unsigned char *dst ,const TpresetSettings *cfg); 
 void noiseAvihUV (const unsigned char *srcU,unsigned char *dstU,const unsigned char *srcV,unsigned char *dstV,const TpresetSettings *cfg);
protected:
 virtual void done(void);
public:
 TimgFilterNoise(void);
 virtual void init(int Idx,int Istride,int Idy);
 virtual void process(const unsigned char *srcY,const unsigned char *srcU,const unsigned char *srcV,
                      unsigned char *dstY,unsigned char *dstU,unsigned char *dstV,
                      const TpresetSettings *cfg);
};

#endif
