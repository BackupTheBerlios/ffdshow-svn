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

#include <windows.h>
#pragma hdrstop
#include <math.h>
#include "TimgFilterLuma.h"
#include "TpresetSettings.h"

const int TpresetSettings::lumGainDef=128,TpresetSettings::lumOffsetDef=0,TpresetSettings::gammaCorrectionDef=100;

TimgFilterLuma::TimgFilterLuma(void)
{
 oldGamma=-1;
}

void TimgFilterLuma::process(TtempPictures *pict,TffRect &rect,const TpresetSettings *cfg)
{
 if (cfg->lumGain==TpresetSettings::lumGainDef && cfg->lumOffset==TpresetSettings::lumOffsetDef && cfg->gammaCorrection==TpresetSettings::gammaCorrectionDef) return;
 TffRect::Trect *r=init(&rect,0);
 const unsigned char *srcY=pict->getCurY()+r->diffY;unsigned char *dstY=pict->getNextY()+r->diffY;

 const unsigned char *gammaSrc;unsigned char *gammaDst;
 if (cfg->lumGain==cfg->lumGainDef && cfg->lumOffset==cfg->lumOffsetDef)
  {
   gammaSrc=srcY;gammaDst=dstY;
  }
 else 
  {
   gammaSrc=dstY;gammaDst=dstY;
   __declspec(align(8)) static const __int64 m64 =0x0040004000400040;
   __declspec(align(8)) static __int64 lumGainMask,lumOffsetMask;
   __int64 lumGain=cfg->lumGain,lumOffset=cfg->lumOffset;
   lumGainMask=(lumGain<<48) + (lumGain<<32) + (lumGain<<16) + lumGain;
   lumOffsetMask=(lumOffset<<48) + (lumOffset<<32) + (lumOffset<<16) + lumOffset;
   int LUM_AREA=dxY;
   const unsigned char *src,*srcYend;unsigned char *dst;
   for (src=srcY,srcYend=srcY+strideY*dyY,dst=dstY;src<srcYend;src+=strideY,dst+=strideY)
    {
     __asm
      {
       mov         eax, [src]
       mov         ebx, [dst]
       mov         edx, 0x00       
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
       movq        [ebx+edx-8], mm1
       movq        [ebx+edx-4], mm2
       jl          lumconv
      };
    };  
  }
 __asm emms;
 if (cfg->gammaCorrection!=oldGamma)
  {
   oldGamma=cfg->gammaCorrection;
   double gamma=100.0/(oldGamma);
   for(int i=0;i<256;i++)
    gammaTab[i]=(unsigned char)(255.0*pow((i/255.0),gamma));
  };
 if (cfg->gammaCorrection!=cfg->gammaCorrectionDef)
  for (int y=0;y<dyY;y++)
   {
    const unsigned char *src;unsigned char *dst,*dstEnd;
    for (src=gammaSrc+strideY*y,dst=gammaDst+strideY*y,dstEnd=dst+dxY;dst<dstEnd;src+=4,dst+=4)
     *(unsigned int*)dst=(gammaTab[src[3]]<<24)|(gammaTab[src[2]]<<16)|(gammaTab[src[1]]<<8)|gammaTab[src[0]];
   }  
};
