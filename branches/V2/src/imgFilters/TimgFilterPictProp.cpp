/*
 * Copyright (c) 2002 Milan Cutka
 * luminance gain and offset based on DVD2AVI source code
 * hue and saturation based on AVIsynth filter by Donald A. Graft
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
#include "TimgFilterPictProp.h"
#include "TfilterPictProp.h"

TimgFilterPictProp::TimgFilterPictProp(void)
{
 oldGamma=-1;
 for (int i=-180,ii=0;i<=180;ii++,i++)
  {
   double Hue=(i*3.1415926)/180.0;
   hueSin[ii]=int(sin(Hue)*128);
   hueCos[ii]=int(cos(Hue)*128);
  }
}

void TimgFilterPictProp::processLuma(const unsigned char *srcY,unsigned char *dstY,const TfilterPictProp *cfg)
{
 static __declspec(align(8)) const __int64 m64 =0x0040004000400040;
 static __declspec(align(8)) __int64 lumGainMask,lumOffsetMask;
 __int64 lumGain=cfg->settings.lumGain,lumOffset=cfg->settings.lumOffset;
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
}

void TimgFilterPictProp::processGamma(const unsigned char *srcY,unsigned char *dstY,const TfilterPictProp *cfg)
{
 if (cfg->settings.gammaCorrection!=oldGamma)
  {
   __asm emms;
   oldGamma=cfg->settings.gammaCorrection;
   double gamma=100.0/(oldGamma);
   for(int i=0;i<256;i++)
    gammaTab[i]=(unsigned char)(255.0*pow((i/255.0),gamma));
  }
 if (cfg->settings.gammaCorrection!=cfg->gammaCorrectionDef)
  for (unsigned int y=0;y<dyY;y++)
   {
    const unsigned char *src;unsigned char *dst,*dstEnd;
    for (src=srcY+strideY*y,dst=dstY+strideY*y,dstEnd=dst+dxY;dst<dstEnd;src+=4,dst+=4)
     *(unsigned int*)dst=(gammaTab[src[3]]<<24)|(gammaTab[src[2]]<<16)|(gammaTab[src[1]]<<8)|gammaTab[src[0]];
   }  
}

void TimgFilterPictProp::processChroma(const unsigned char *srcU,const unsigned char *srcV,unsigned char *dstU,unsigned char *dstV,const TfilterPictProp *cfg)
{
 int hue=cfg->settings.hue;          //-180 ... 0 ... 180
 int sat=cfg->settings.saturation ;//0 (BW) - 64 (normal) - 128 (too much color);
 //#define CHUESAT
 #ifdef CHUESAT
 int Sin=hueSin[hue+180];// (int) (sin(Hue) * 4096);
 int Cos=hueCos[hue+180];// (int) (cos(Hue) * 4096);
 int diffx=stride-dx;
 #else
 static __declspec(align(8)) const __int64 m128=0x0080008000800080;
 __int64 Sin=hueSin[hue+180];
 __int64 Cos=hueCos[hue+180];
 __int64 Sat=sat;
 static __declspec(align(8)) __int64 Sin64,Cos64,Sat64;
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
  }
 __asm emms; 
}

void TimgFilterPictProp::process(TffPict2 &pict,const Tfilter *cfg0)
{
 const TfilterPictProp *cfg=(const TfilterPictProp*)cfg0;
 if (!cfg->is) return;
 Trect *r=init(&pict.rect,cfg->full);
 if (cfg->settings.lumGain!=cfg->lumGainDef || cfg->settings.lumOffset!=cfg->lumOffsetDef)
  { 
   const unsigned char *srcY=getCurY(pict)+r->diffY;unsigned char *dstY=getCurNextY(pict)+r->diffY;
   processLuma(srcY,dstY,cfg);
  }; 
 if (cfg->settings.gammaCorrection!=cfg->gammaCorrectionDef)
  {
   const unsigned char *srcY=getCurY(pict)+r->diffY;unsigned char *dstY=getCurNextY(pict)+r->diffY;
   processGamma(srcY,dstY,cfg);
  }
 if (cfg->settings.hue!=cfg->hueDef || cfg->settings.saturation!=cfg->saturationDef)
  {
   const unsigned char *srcU=getCurU(pict)+r->diffUV;unsigned char *dstU=getCurNextU(pict)+r->diffUV;
   const unsigned char *srcV=getCurV(pict)+r->diffUV;unsigned char *dstV=getCurNextV(pict)+r->diffUV;
   processChroma(srcU,srcV,dstU,dstV,cfg);
  } 
}
