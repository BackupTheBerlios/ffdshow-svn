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

#pragma hdrstop
#include <math.h>
#include "TimgFilterLuma.h"
#include "TpresetSettings.h"

__declspec(align(8)) static const __int64 m64 =0x0040004000400040;
const int TpresetSettings::lumGainDef=128,TpresetSettings::lumOffsetDef=0,TpresetSettings::gammaCorrectionDef=100;

TimgFilterLuma::TimgFilterLuma(void)
{
 oldGamma=-1;
}

void TimgFilterLuma::process(unsigned char *srcY,unsigned char *,unsigned char *,
                             unsigned char *dstY,unsigned char *,unsigned char *,
                             TpresetSettings *cfg)
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
 __asm emms;
 if (cfg->gammaCorrection!=oldGamma)
  {
   oldGamma=cfg->gammaCorrection;
   //calc123(oldGamma);
   double gamma=100.0/(oldGamma);
   for(int i=0;i<256;i++)
    gammaTab[i]=(unsigned char)(255.0*pow((i/255.0),gamma));
  };
 if (cfg->gammaCorrection!=cfg->gammaCorrectionDef)
  for (int y=0;y<dyY;y++)
   for (unsigned char *dst=dstY+strideY*y,*dstEnd=dst+dxY;dst<dstEnd;dst++)
    *dst=gammaTab[*dst];
/*    
    { 
     float c=*dst/255.0;
     c=c3*c*c*c+c2*c*c+c1*c;
     if (c<0) c=0;if (c>1) c=1;
     *dst=c*255;
    }; 
*/     
};
void TimgFilterLuma::calc123(int Ig0)
{
 __asm emms;
 float g0=Ig0/100.0;
 float g=(g0<1)?1/g0:g0;
 unsigned int i;
 float x[256],exp[256];
 for (i=0;i<256;i++)
  exp[i]=pow(i/255.0,g);

 float s0=0,s1=0,t0=0,s2=0,s3=0,s4=0,s5=0,s6=0,t1=0,t2=0,t3=0;
 for (i=0;i<256;i++)
  {
   s0=s0+i; //i
   float xx=i/255.0;
   s1=s1+xx; //x
   float yy=exp[i];
   t0=t0+yy; //y
   s2=s2+xx*xx;
   s3=s3+xx*xx*xx;
   s4=s4+xx*xx*xx*xx;
   s5=s5+xx*xx*xx*xx*xx;
   s6=s6+xx*xx*xx*xx*xx*xx;
   t1=t1+yy*xx;
   t2=t2+yy*xx*xx;
   t3=t3+yy*xx*xx*xx;
  };
 float a11=s1-s3,a12=s2-s3,a13=s3-t0;
 float a21=s2-s4,a22=s3-s4,a23=s4-t1;
 float _c2=(a11*a23-a13*a21)/(a12*a21-a11*a22);
 float _c1=-(a22*c2+a23)/a21;
 float _c3=1-(c1+c2);
}