#ifndef _TIMGFILTERNOISE_H_
#define _TIMGFILTERNOISE_H_

#include "TimgFilter.h"

struct TfilterNoise;
class TimgFilterNoise :public TimgFilter
{
private:
 int noiseAvihStrength,noiseAvihStrengthChroma;
 short *noiseMaskY;int noiseCountY;
 short *noiseMaskU;int noiseCountU;
 short *noiseMaskV;int noiseCountV;
 void noise0luma  (const unsigned char *src ,unsigned char *dst ,unsigned int stride,unsigned int dx,unsigned int dy,int noiseStrength,int uniformNoise,short *noiseMask,int noiseCount);
 void noise0chroma(const unsigned char *src ,unsigned char *dst ,unsigned int stride,unsigned int dx,unsigned int dy,int noiseStrength,int uniformNoise,short *noiseMask,int noiseCount);
 void noiseY      (const unsigned char *src ,unsigned char *dst ,const TfilterNoise *cfg); 
 void noiseUV     (const unsigned char *srcU,unsigned char *dstU,const unsigned char *srcV,unsigned char *dstV,const TfilterNoise *cfg);
 void noiseAvihY  (const unsigned char *src ,unsigned char *dst ,const TfilterNoise *cfg); 
 void noiseAvihUV (const unsigned char *srcU,unsigned char *dstU,const unsigned char *srcV,unsigned char *dstV,const TfilterNoise *cfg);
protected:
 virtual Trect* init(TffRect *rect,int full);
public:
 TimgFilterNoise(void);
 virtual void done(void);
 virtual void process(TffPict2 &pict,const Tfilter *cfg0);
};

#endif
