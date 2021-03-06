/*
 * Copyright (c) 2002 Milan Cutka
 * luminance gain and offset based on DVD2AVI source code
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
#include "TimgFilterLuma.h"
#include "TpresetSettings.h"

const int TpresetSettings::TpictPropSettings::lumGainDef=128,TpresetSettings::TpictPropSettings::lumOffsetDef=0,TpresetSettings::TpictPropSettings::gammaCorrectionDef=100;

TimgFilterLuma::TimgFilterLuma(void)
{
 oldGamma=-1;
}

void TimgFilterLuma::processGainOffset(const unsigned char *srcY,unsigned char *dstY,const TpresetSettings *cfg)
{
 static __declspec(align(8)) const __int64 m64 =0x0040004000400040;
 static __declspec(align(8)) __int64 lumGainMask,lumOffsetMask;
 __int64 lumGain=cfg->pictProp.lumGain,lumOffset=cfg->pictProp.lumOffset;
 lumGainMask  =(lumGain  <<48) + (lumGain  <<32) + (lumGain  <<16) + lumGain  ;
 lumOffsetMask=(lumOffset<<48) + (lumOffset<<32) + (lumOffset<<16) + lumOffset;
 int LUM_AREA=dxY;
 const unsigned char *src,*srcYend;unsigned char *dst;
 for (src=srcY,srcYend=srcY+strideY*dyY,dst=dstY;src<srcYend;src+=strideY,dst+=strideY)
  {
   __asm
    {
     mov         eax, [src]
     mov         esi, [dst]
     mov         edx, 0
     mov         edi, [LUM_AREA]
     pxor        mm0, mm0
     movq        mm5, [lumOffsetMask]
     movq        mm6, [lumGainMask]
     movq        mm7, [m64]

    lumconv:
     movq        mm1, [eax+edx]
     movq        mm2, mm1              
     
     punpcklbw   mm1, mm0
     punpckhbw   mm2, mm0

     pmullw      mm1, mm6
     pmullw      mm2, mm6

     paddw       mm1, mm7
     paddw       mm2, mm7

     psrlw       mm1, 7
     psrlw       mm2, 7

     paddw       mm1, mm5
     paddw       mm2, mm5

     packuswb    mm1, mm0
     packuswb    mm2, mm0

     add         edx, 0x08
     cmp         edx, edi
     movd        [esi+edx-8], mm1
     movd        [esi+edx-4], mm2
     jl          lumconv
    }
  }
 __asm emms;
};

void TimgFilterLuma::processGamma(const unsigned char *srcY,unsigned char *dstY,const TpresetSettings *cfg)
{
 if (cfg->pictProp.gammaCorrection!=oldGamma)
  {
   oldGamma=cfg->pictProp.gammaCorrection;
   __asm emms;
   double gamma=100.0/(oldGamma);
   for(int i=0;i<256;i++)
    gammaTab[i]=(unsigned char)(255.0*pow((i/255.0),gamma));
  }
 if (cfg->pictProp.gammaCorrection!=TpresetSettings::TpictPropSettings::gammaCorrectionDef)
  for (unsigned int y=0;y<dyY;y++)
   {
    const unsigned char *src;unsigned char *dst,*dstEnd;
    for (src=srcY+strideY*y,dst=dstY+strideY*y,dstEnd=dst+dxY;dst<dstEnd;src+=4,dst+=4)
     *(unsigned int*)dst=(gammaTab[src[3]]<<24)|(gammaTab[src[2]]<<16)|(gammaTab[src[1]]<<8)|gammaTab[src[0]];
   }  
}

void TimgFilterLuma::process(TffPict2 &pict,const TpresetSettings *cfg)
{
 if (cfg->pictProp.lumGain==cfg->pictProp.lumGainDef && cfg->pictProp.lumOffset==cfg->pictProp.lumOffsetDef && cfg->pictProp.gammaCorrection==cfg->pictProp.gammaCorrectionDef) return;
 Trect *r=init(&pict.rect,cfg->fullPictProp);
 if (cfg->pictProp.lumGain!=cfg->pictProp.lumGainDef || cfg->pictProp.lumOffset!=cfg->pictProp.lumOffsetDef)
  {
   const unsigned char *srcY=getCurY(pict)+r->diffY;unsigned char *dstY=getCurNextY(pict)+r->diffY;
   processGainOffset(srcY,dstY,cfg);
  }
 if (cfg->pictProp.gammaCorrection!=cfg->pictProp.gammaCorrectionDef)
  {
   const unsigned char *srcY=getCurY(pict)+r->diffY;unsigned char *dstY=getCurNextY(pict)+r->diffY;
   processGamma(srcY,dstY,cfg);
  }
}