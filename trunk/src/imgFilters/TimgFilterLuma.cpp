/*
 * Copyright (c) 2002 Milan Cutka
 * luminance gain and offset base on DVD2AVI source code
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

#include "TimgFilterLuma.h"
#include "..\Tconfig.h"
#include <math.h>

__declspec(align(8)) static const __int64 m64 =0x0040004000400040;
const int Tconfig::lumGainDef=128,Tconfig::lumOffsetDef=0,Tconfig::gammaCorrectionDef=100;

TimgFilterLuma::TimgFilterLuma(void)
{
 oldGamma=-1;
}

void TimgFilterLuma::process(unsigned char *srcY,unsigned char *srcU,unsigned char *srcV,
                             unsigned char *dstY,unsigned char *dstU,unsigned char *dstV,
                             Tconfig *cfg)
{
 __declspec(align(8)) static __int64 lumGainMask;
 lumGainMask=((__int64)cfg->lumGain<<48) + ((__int64)cfg->lumGain<<32) + ((__int64)cfg->lumGain<<16) + (__int64)cfg->lumGain;
 __declspec(align(8)) static __int64 lumOffsetMask;
 lumOffsetMask=((__int64)cfg->lumOffset<<48) + ((__int64)cfg->lumOffset<<32) + ((__int64)cfg->lumOffset<<16) + (__int64)cfg->lumOffset;
 for (unsigned char *src=srcY,*srcYend=src+strideY*dyY,*dst=dstY;src<srcYend;src+=strideY,dst+=strideY)
  {
   int LUM_AREA=dxY;
   __asm
    {
     mov         eax, [src]
     mov         ebx, [dst]
     mov         esi, 0x00
     mov         edi, [LUM_AREA]
     pxor        mm0, mm0
     movq        mm5, [lumOffsetMask]
     movq        mm6, [lumGainMask]
     movq        mm7, [m64]
 
    lumconv:
     movq        mm1, [eax+esi]
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

     add         esi, 0x08
     cmp         esi, edi
     movq        [ebx+esi-8], mm1
     movq        [ebx+esi-4], mm2
     jl          lumconv
    };
  };  
 if (cfg->gammaCorrection!=oldGamma)
  {
   __asm emms;
   float gamma=100.0/(oldGamma=cfg->gammaCorrection);
   for(int i=0;i<256;i++)
    gammaTab[i]=255.0*pow((i/255.0),gamma);
  };
 if (cfg->gammaCorrection!=cfg->gammaCorrectionDef)
  for (int y=0;y<dyY;y++)
   for (unsigned char *dst=dstY+strideY*y,*dstEnd=dst+dxY;dst<dstEnd;dst++)
    *dst=gammaTab[*dst];
};
