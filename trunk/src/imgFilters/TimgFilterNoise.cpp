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

#include "stdafx.h"
#include "TimgFilterNoise.h" 
#include "xvid\utils\mem_align.h"
#include "TpresetSettings.h"
#include <time.h>

const int TpresetSettings::TnoiseSettings::strengthDef=15,TpresetSettings::TnoiseSettings::strengthChromaDef=0;

static __declspec(align(8)) const __int64 m128=0x0080008000800080;
static __declspec(align(8)) const __int64 m255=0x00ff00ff00ff00ff;
static __declspec(align(8)) const unsigned __int64 noiseConst=6364136223846793005i64;
static __declspec(align(8)) unsigned __int64 noisenext;

TimgFilterNoise::TimgFilterNoise(void)
{
 noiseMaskY=noiseMaskU=noiseMaskV=NULL;
}
Trect* TimgFilterNoise::init(TffRect *rect,int full)
{
 Trect *r=TimgFilter::init(rect,full);
 if (!noiseMaskY)
  {
   noiseCountY=noiseCountU=noiseCountV=-1;
   noiseAvihStrength=0;
   noiseMaskY=(short*)xvid_calloc(dxY *dyY *2,2,MCACHE_LINE);
   noiseMaskU=(short*)xvid_calloc(dxUV*dyUV*2,2,MCACHE_LINE);
   noiseMaskV=(short*)xvid_calloc(dxUV*dyUV*2,2,MCACHE_LINE);
   noisenext=time(NULL);
  }
 return r;
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
void TimgFilterNoise::noiseY(const unsigned char *src,unsigned char *dst,const TpresetSettings *cfg)
{
 noiseCountY++;
 if (noiseAvihStrength) noiseCountY=0;
 noise0luma(src,dst,strideY,dxY,dyY,cfg->noise.strength,cfg->noise.uniformNoise,noiseMaskY,noiseCountY);
 noiseAvihStrength=0;
}
void TimgFilterNoise::noiseUV(const unsigned char *srcU,unsigned char *dstU,const unsigned char *srcV,unsigned char *dstV,const TpresetSettings *cfg)
{
 noiseCountU++;noiseCountV++;
 if (noiseAvihStrengthChroma) noiseCountU=noiseCountV=0;
 noise0chroma(srcU,dstU,strideUV,dxUV,dyUV,cfg->noise.strengthChroma,/*cfg->uniformNoise*/true,noiseMaskU,noiseCountU);
 noise0chroma(srcV,dstV,strideUV,dxUV,dyUV,cfg->noise.strengthChroma,/*cfg->uniformNoise*/true,noiseMaskV,noiseCountV);
 noiseAvihStrengthChroma=0;
}

void TimgFilterNoise::noise0luma(const unsigned char *src,unsigned char *dst,unsigned int stride,unsigned int dx,unsigned int dy,int noiseStrength,int uniformNoise,short *noiseMask,int noiseCount)
{
 #undef NOISE_CHROMA
 #undef NOISE_AVIH 
 #include "noise_template.h"
}

void TimgFilterNoise::noise0chroma(const unsigned char *src,unsigned char *dst,unsigned int stride,unsigned int dx,unsigned int dy,int noiseStrength,int uniformNoise,short *noiseMask,int noiseCount)
{
 #define NOISE_CHROMA
 #undef NOISE_AVIH 
 #include "noise_template.h"
}

void TimgFilterNoise::noiseAvihY(const unsigned char *src,unsigned char *dst,const TpresetSettings *cfg)
{
 if (cfg->noise.strength!=noiseAvihStrength)
  {
   noiseAvihStrength=cfg->noise.strength;
   short *dst1=noiseMaskY,*dst2=dst1+dxY*dyY;
   for (unsigned int i=0;i<dxY*dyY;i++)
    dst1[i]=dst2[i]=(((rand()&255)-128)*noiseAvihStrength)/256;
  }
 #define NOISE_AVIH 
 #undef NOISE_CHROMA
 int stride=strideY,dx=dxY,dy=dyY;
 short *noiseMaskPtr=noiseMaskY+rand()%(dxY*dyY); 
 #include "noise_avih_template.h"
 __asm emms;
}
void TimgFilterNoise::noiseAvihUV(const unsigned char *srcU,unsigned char *dstU,const unsigned char *srcV,unsigned char *dstV,const TpresetSettings *cfg)
{
 if (cfg->noise.strengthChroma!=noiseAvihStrengthChroma)
  {
   noiseAvihStrengthChroma=cfg->noise.strengthChroma;
   short *dst1=noiseMaskU,*dst2=dst1+dxUV*dyUV;
   unsigned int i;
   for (i=0;i<dxUV*dyUV;i++)
    dst1[i]=dst2[i]=(((rand()&255)-128)*noiseAvihStrengthChroma)/256;
   dst1=noiseMaskV;dst2=dst1+dxUV*dyUV; 
   for (i=0;i<dxUV*dyUV;i++)
    dst1[i]=dst2[i]=(((rand()&255)-128)*noiseAvihStrengthChroma)/256;
  }
 #define NOISE_AVIH 
 #undef NOISE_CHROMA
 const unsigned char *src=srcU;unsigned char *dst=dstU;
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
 __asm emms;
}

void TimgFilterNoise::process(TffPict2 &pict,const TpresetSettings *cfg)
{
 if (!cfg->noise.strength && !cfg->noise.strengthChroma) return;
 Trect *r=init(&pict.rect,cfg->fullNoise);
 if (cfg->noise.strength)
  {  
   const unsigned char *srcY=getCurY(pict)+r->diffY;unsigned char *dstY=getCurNextY(pict)+r->diffY;
   if (cfg->noise.method==0) 
    noiseY(srcY,dstY,cfg);
   else 
    noiseAvihY(srcY,dstY,cfg);
   __asm emms;
  }
 if (cfg->noise.strengthChroma)
  {
   const unsigned char *srcU=getCurU(pict)+r->diffUV;unsigned char *dstU=getCurNextU(pict)+r->diffUV;
   const unsigned char *srcV=getCurV(pict)+r->diffUV;unsigned char *dstV=getCurNextV(pict)+r->diffUV;
   if (cfg->noise.method==0)
    noiseUV(srcU,dstU,srcV,dstV,cfg);
   else 
    noiseAvihUV(srcU,dstU,srcV,dstV,cfg);
   __asm emms;
  }
}
