/*
 * Copyright (c) 2002 Milan Cutka
 * noise algorithm 1 by avih
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "TimgFilterNoise.h" 
#include <stdlib.h>
#include "xvid\utils\mem_align.h"
#include "TpresetSettings.h"
#include <time.h>

const int TpresetSettings::noiseStrengthDef=15,TpresetSettings::noiseStrengthChromaDef=0;

__declspec(align(8)) static const __int64 m128=0x0080008000800080;
__declspec(align(8)) static const __int64 m255=0x00ff00ff00ff00ff;
__declspec(align(8)) static const unsigned __int64 noiseConst=6364136223846793005i64;
__declspec(align(8)) static unsigned __int64 noisenext;

TimgFilterNoise::TimgFilterNoise(void)
{
 noiseMaskY=noiseMaskU=noiseMaskV=NULL;
}
void TimgFilterNoise::init(int Idx,int Istride,int Idy)
{
 TimgFilter::init(Idx,Istride,Idy);
 noiseCountY=noiseCountU=noiseCountV=-1;
 noiseAvihStrenth=0;
 noiseMaskY=(short*)xvid_calloc(dxY*dyY*2,2,MCACHE_LINE);
 noiseMaskU=(short*)xvid_calloc(dxUV*dyUV*2,2,MCACHE_LINE);
 noiseMaskV=(short*)xvid_calloc(dxUV*dyUV*2,2,MCACHE_LINE);
 time_t t;  
 time(&t); 
 noisenext=t;
 
}
void TimgFilterNoise::done(void)
{
 if (noiseMaskY)
  {
   xvid_free(noiseMaskY);
   noiseMaskY=NULL;
  } 
 if (noiseMaskU)
  {
   xvid_free(noiseMaskU);
   noiseMaskU=NULL;
  } 
 if (noiseMaskV)
  {
   xvid_free(noiseMaskV);
   noiseMaskV=NULL;
  } 
}
void TimgFilterNoise::noiseY(unsigned char *src,unsigned char *dst,TpresetSettings *cfg)
{
 noiseCountY++;
 if (noiseAvihStrenth) noiseCountY=0;
 noise0luma(src,dst,strideY,dxY,dyY,cfg->noiseStrength,cfg->uniformNoise,noiseMaskY,noiseCountY);
 noiseAvihStrenth=0;
}
void TimgFilterNoise::noiseUV(unsigned char *srcU,unsigned char *dstU,unsigned char *srcV,unsigned char *dstV,TpresetSettings *cfg)
{
 noiseCountU++;noiseCountV++;
 if (noiseAvihStrenthChroma) noiseCountU=noiseCountV=0;
 noise0chroma(srcU,dstU,strideUV,dxUV,dyUV,cfg->noiseStrengthChroma,/*cfg->uniformNoise*/true,noiseMaskU,noiseCountU);
 noise0chroma(srcV,dstV,strideUV,dxUV,dyUV,cfg->noiseStrengthChroma,/*cfg->uniformNoise*/true,noiseMaskV,noiseCountV);
 noiseAvihStrenthChroma=0;
}

void TimgFilterNoise::noise0luma(unsigned char *src,unsigned char *dst,int stride,int dx,int dy,int noiseStrength,int uniformNoise,short *noiseMask,int noiseCount)
{
 #undef NOISE_CHROMA
 #undef NOISE_AVIH 
 #include "noise_template.h"
}

void TimgFilterNoise::noise0chroma(unsigned char *src,unsigned char *dst,int stride,int dx,int dy,int noiseStrength,int uniformNoise,short *noiseMask,int noiseCount)
{
 #define NOISE_CHROMA
 #undef NOISE_AVIH 
 #include "noise_template.h"
}

void TimgFilterNoise::noiseAvihY(unsigned char *src,unsigned char *dst,TpresetSettings *cfg)
{
 if (cfg->noiseStrength!=noiseAvihStrenth)
  {
   noiseAvihStrenth=cfg->noiseStrength;
   short *dst1=noiseMaskY,*dst2=dst1+dxY*dyY;
   for (int i=0;i<dxY*dyY;i++)
    dst1[i]=dst2[i]=(((rand()&255)-128)*noiseAvihStrenth)/256;
  }
 #define NOISE_AVIH 
 #undef NOISE_CHROMA
 int stride=strideY,dx=dxY,dy=dyY;
 short *noiseMaskPtr=noiseMaskY+rand()%(dxY*dyY); 
 #include "noise_avih_template.h"
 __asm {emms};
}
void TimgFilterNoise::noiseAvihUV(unsigned char *srcU,unsigned char *dstU,unsigned char *srcV,unsigned char *dstV,TpresetSettings *cfg)
{
 if (cfg->noiseStrengthChroma!=noiseAvihStrenthChroma)
  {
   noiseAvihStrenthChroma=cfg->noiseStrengthChroma;
   short *dst1=noiseMaskU,*dst2=dst1+dxUV*dyUV;
   int i;
   for (i=0;i<dxUV*dyUV;i++)
    dst1[i]=dst2[i]=(((rand()&255)-128)*noiseAvihStrenthChroma)/256;
   dst1=noiseMaskV;dst2=dst1+dxUV*dyUV; 
   for (i=0;i<dxUV*dyUV;i++)
    dst1[i]=dst2[i]=(((rand()&255)-128)*noiseAvihStrenthChroma)/256;
  }
 #define NOISE_AVIH 
 #undef NOISE_CHROMA
 unsigned char *src=srcU,*dst=dstU;
 int dx=dxUV,stride=strideUV,dy=dyUV;
 short *noiseMaskPtr=noiseMaskU+rand()%(dx*dy); 
 #define lineLoop2u lineLoop2u1
 #define lineLoop2nu lineLoop2nu1
 #include "noise_avih_template.h"
 src=srcV;dst=dstV;
 noiseMaskPtr=noiseMaskV+rand()%(dx*dy); 
 #undef  lineLoop2u
 #define lineLoop2u lineLoop2u2
 #undef  lineLoop2nu
 #define lineLoop2nu lineLoop2nu2
 #include "noise_avih_template.h"
 __asm {emms};
}

void TimgFilterNoise::process(unsigned char *srcY,unsigned char *srcU,unsigned char *srcV,
                              unsigned char *dstY,unsigned char *dstU,unsigned char *dstV,
                              TpresetSettings *cfg)
{
 if (srcY && dstY)
  {
   if (cfg->noiseMethod==0) 
    noiseY(srcY,dstY,cfg);
   else 
    noiseAvihY(srcY,dstY,cfg);
   return; 
  }
 if (cfg->noiseMethod==0)                    
  noiseUV(srcU,dstU,srcV,dstV,cfg);
 else 
  noiseAvihUV(srcU,dstU,srcV,dstV,cfg);
};
                               
