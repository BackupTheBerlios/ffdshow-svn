/*
 * Copyright (c) 2002 Milan Cutka
 * based on AVIsynth filter by Donald A. Graft
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
#include "TimgFilterChroma.h"
#include "TpresetSettings.h"

const int TpresetSettings::hueDef=0,TpresetSettings::saturationDef=64;

TimgFilterChroma::TimgFilterChroma(void)
{
 __asm emms;
 for (int i=-180,ii=0;i<=180;ii++,i++)
  {
   double Hue=(i * 3.1415926) / 180.0;
   hueSin[ii]=int(sin(Hue) * 128);
   hueCos[ii]=int(cos(Hue) * 128);
  }
}

void TimgFilterChroma::process(TffPict *pict,TffRect &rect,const TpresetSettings *cfg)
{
 if (cfg->hue==TpresetSettings::hueDef && cfg->saturation==TpresetSettings::saturationDef) return;
 TffRect::Trect *r=init(&rect,0);
 const unsigned char *srcU=pict->getCurU()+r->diffUV;unsigned char *dstU=pict->getNextU()+r->diffUV;
 const unsigned char *srcV=pict->getCurV()+r->diffUV;unsigned char *dstV=pict->getNextV()+r->diffUV;
 
 int hue=cfg->hue;          //-180 ... 0 ... 180
 int sat = cfg->saturation ;//0 (BW) - 64 (normal) - 128 (too much color);
 //#define CHUESAT
 #ifdef CHUESAT
 int Sin = hueSin[hue+180];// (int) (sin(Hue) * 4096);
 int Cos = hueCos[hue+180];// (int) (cos(Hue) * 4096);
 int diffx=stride-dx;
 #else
 __declspec(align(8)) static const __int64 m128=0x0080008000800080;
 __int64 Sin = hueSin[hue+180];
 __int64 Cos = hueCos[hue+180];
 __int64 Sat = sat;
 __declspec(align(8)) static __int64 Sin64,Cos64,Sat64;
 Sin64=(Sin&0x000000000000ffff)+((Sin<<16)&0x00000000ffff0000)+((Sin<<32)&0x0000ffff00000000)+((Sin<<48)&0xffff000000000000);
 Cos64=(Cos&0x000000000000ffff)+((Cos<<16)&0x00000000ffff0000)+((Cos<<32)&0x0000ffff00000000)+((Cos<<48)&0xffff000000000000);
 Sat64=(Sat&0x000000000000ffff)+((Sat<<16)&0x00000000ffff0000)+((Sat<<32)&0x0000ffff00000000)+((Sat<<48)&0xffff000000000000);
 #endif
 for (const unsigned char *srcUend=srcU+strideUV*dyUV;srcU<srcUend;srcU+=strideUV,srcV+=strideUV,dstU+=strideUV,dstV+=strideUV)
  {
   const unsigned char *srcUlnEnd=srcU+dxUV;
   #ifdef CHUESAT
   stride=diffx;
   for (;srcU<srcUlnEnd;srcU++,srcV++,dstU++,dstV++)
    {
     int u = *srcU - 128;
     int v = *srcV - 128;
     int u2 = ((u * Cos)>>7) + ((v * Sin)>>7);
     int v2 = ((v * Cos)>>7) - ((u * Sin)>>7);
     u = ((u2 * sat) >> 6) + 128;
     v = ((v2 * sat) >> 6) + 128;
     if (u < 0) u = 0;
     if (u > 255) u = 255;
     if (v < 0) v = 0;
     if (v > 255) v = 255;
     *dstU = u;
     *dstV = v;
    } 
   #else
   //srcU[0]=1;srcU[1]=2;srcU[2]=3;srcU[3]=4;
   //srcV[0]=5;srcV[1]=6;srcV[2]=7;srcV[3]=8;
   __asm 
    {
     mov eax,[srcUlnEnd]
     mov ecx,[srcU]
     mov esi,[srcV]
     mov edx,[dstU]
     mov edi,[dstV]
     movq mm0,[Sat64] // mm0 = Sat64
     movq mm1,[Sin64] // mm1 = Sin64
     movq mm2,[Cos64] // mm2 = Cos64
    lineLoop:
     pxor mm3,mm3
     punpcklbw mm3,[ecx]  
     psrlw mm3,8     //mm3 = *srcU        
     
     pxor mm4,mm4
     punpcklbw mm4,[esi]  
     psrlw mm4,8     //mm4 = *srcV        

     movq mm7,[m128]
     PSUBSW mm3,mm7  //mm3 = u = *srcU - 128
     PSUBSW mm4,mm7  //mm4 = v = *srcV - 128
     
     movq mm5,mm3
     PMULLW mm5,mm2     //mm5 = u * Cos
     PSRAW mm5,7        //mm5 = u2 = (u * Cos + v * Sin)>>12
     movq mm6,mm4
     PMULLW mm6,mm1     //mm6 = v * Sin
     PSRAW mm6,7
     paddw mm5,mm6      //mm5 = u * Cos + v * Sin
     PMULLW mm5,mm0     //mm5 = u2 * Sat
     PSRAW mm5,6        //mm5 =(u2 * Sat)>>8
     PADDW mm5,[m128]   //mm5 =(u2 * Sat)>>8 +128
     
     movq mm7,mm4
     PMULLW mm7,mm2     //mm7 = v * Cos
     PSRAW mm7,7
     movq mm6,mm3
     PMULLW mm6,mm1     //mm6 = u * Sin
     PSRAW mm6,7        //mm7 = v2 = (v * Cos + u * Sin)>>12
     psubw mm7,mm6      //mm7 = v * Cos - u * Sin
     PMULLW mm7,mm0     //mm7 = v2 * Sat
     PSRAW mm7,6        //mm7 =(v2 * Sat)>>8
     PADDW mm7,[m128]   //mm7 =(v2 * Sat)>>8 +128
     
     pxor mm6,mm6
     packuswb mm5,mm6
     movd [edx],mm5
     pxor mm6,mm6
     packuswb mm7,mm6
     movd [edi],mm7
     add ecx,4
     add esi,4
     add edx,4
     add edi,4
     cmp ecx,eax
     jl  lineLoop
    }
   #endif 
  }; 
 __asm emms; 
}
