/*
 * Copyright (c) 2002 Milan Cutka
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
#include "TimgFilterBlur.h"
#include "TfilterBlur.h"
#include "xvid\utils\mem_align.h"
#include "ffdebug.h"

TimgFilterBlur::TimgFilterBlur(void)
{
 oldBlurStrength=-1;
 blurTempPict=NULL;
 oldTimesmoothStrength=-1;
 framecount=0;
 accumY=accumU=accumV=NULL;
}
void TimgFilterBlur::done(void)
{
 if (blurTempPict) xvid_free(blurTempPict);blurTempPict=NULL;
 if (accumY) xvid_free(accumY);accumY=NULL;
 if (accumU) xvid_free(accumU);accumU=NULL;
 if (accumV) xvid_free(accumV);accumV=NULL;
}
void TimgFilterBlur::processBlur(const unsigned char *srcY,unsigned char *dstY,const TfilterBlur *cfg)
{
 if (!blurTempPict) blurTempPict=(unsigned char*)xvid_malloc(strideY*dyY,MCACHE_LINE);

 if (oldBlurStrength!=cfg->settings.strength)
  {
   oldBlurStrength=cfg->settings.strength;
   int sum=0;
   sum+=kernel[0]=cfg->settings.strength;
   sum+=kernel[1]=255;
   sum+=kernel[2]=cfg->settings.strength;
   sum=(256*256)/sum;
   kernel[0]=(sum*kernel[0])>>8;
   kernel[1]=(sum*kernel[1])>>8;
   kernel[2]=(sum*kernel[2])>>8;
   kernel[1]+=255-(kernel[0]+kernel[1]+kernel[2]);
  }
 int dx=dxY,dy=dyY,stride=strideY;
 static __declspec(align(8)) __int64 k0;
 static __declspec(align(8)) __int64 k1;
 static __declspec(align(8)) __int64 k2;
 k0=(kernel[0]<<48)+(kernel[0]<<32)+(kernel[0]<<16)+kernel[0];
 k1=(kernel[1]<<48)+(kernel[1]<<32)+(kernel[1]<<16)+kernel[1];
 k2=(kernel[2]<<48)+(kernel[2]<<32)+(kernel[2]<<16)+kernel[2];
 const unsigned char *src,*srcEnd;unsigned char *dst;
 for (src=srcY,srcEnd=srcY+dy*stride,dst=blurTempPict;src<srcEnd;src+=stride,dst+=stride)
  {
   const unsigned char *srcLnEnd=src+dx-1;
   __asm
    {
     mov esi,[src]
     inc esi
     mov edi,[dst]
     inc edi
     mov eax,[srcLnEnd]
     movq mm5,[k0]
     movq mm6,[k1]
     movq mm7,[k2]
    blur1:
     pxor mm0,mm0
     punpcklbw mm0,[esi-1];
     psrlw mm0,8
     pmullw mm0,mm5
     psrlw mm0,8

     pxor mm1,mm1
     punpcklbw mm1,[esi];
     psrlw mm1,8
     pmullw mm1,mm6
     psrlw mm1,8
     paddusb mm0,mm1

     pxor mm2,mm2
     punpcklbw mm2,[esi+1];
     psrlw mm2,8
     pmullw mm2,mm7
     psrlw mm2,8

     paddusb mm0,mm2

     pxor mm1,mm1
     packuswb mm0,mm1
     movq [edi],mm0
     add esi,4
     add edi,4
     cmp esi,eax
     jl  blur1
    }
  }
 const unsigned char *srcL=srcY+stride,*srcR=srcL+dx-1;unsigned char *dstL=blurTempPict+stride,*dstR=dstL+dx-1;
 for (int y=1;y<dy-1;srcL+=stride,srcR+=stride,dstL+=stride,dstR+=stride,y++)
  {
   *dstL=*srcL;
   *dstR=*srcR;
  }
 for (src=blurTempPict+stride,srcEnd=blurTempPict+(dy-1)*stride,dst=dstY+stride;src<srcEnd;src+=stride,dst+=stride)
  {
   const unsigned char *srcLnEnd=src+dx;
   __asm
    {
     mov eax,stride;
     mov esi,[src]
     mov ecx,esi
     sub ecx,eax
     mov edx,esi
     add ecx,eax
     mov edi,[dst]
     mov eax,[srcLnEnd]
     movq mm5,[k0]
     movq mm6,[k1]
     movq mm7,[k2]
    blur2:
     pxor mm0,mm0
     punpcklbw mm0,[ecx];
     psrlw mm0,8
     pmullw mm0,mm5
     psrlw mm0,8

     pxor mm1,mm1
     punpcklbw mm1,[esi];
     psrlw mm1,8
     pmullw mm1,mm6
     psrlw mm1,8
     paddusb mm0,mm1

     pxor mm2,mm2
     punpcklbw mm2,[edx];
     psrlw mm2,8
     pmullw mm2,mm7
     psrlw mm2,8

     paddusb mm0,mm2

     pxor mm1,mm1
     packuswb mm0,mm1
     movq [edi],mm0

     add ecx,4
     add esi,4
     add edx,4
     add edi,4
     cmp esi,eax
     jl  blur2
    }
  }
 memcpy(dstY,blurTempPict,dx);
 memcpy(dstY+strideY*(dy-1),blurTempPict+strideY*(dy-1),dx);
 __asm emms;
}

static const int KERNEL=4;
#define SCALE(i) (0x0000000100010001i64 * (0x10000 / (i)))
#define SCALE2(i) SCALE((i)+0),SCALE((i)+1),SCALE((i)+2),SCALE((i)+3),SCALE((i)+4),\
                  SCALE((i)+5),SCALE((i)+6),SCALE((i)+7),SCALE((i)+8),SCALE((i)+9)

static const __int64 scaletab[]=
{
 0,
 0x00007fff7fff7fffi64,      // special case for 1
 0x00007fff7fff7fffi64,      // special case for 2
 SCALE(3),
 SCALE(4),
 SCALE(5),
 SCALE(6),
 SCALE(7),
 SCALE(8),
 SCALE(9),
 SCALE2(10),
 SCALE2(20),
 SCALE2(30),
 SCALE2(40),
 SCALE2(50),
 SCALE2(60),
 SCALE2(70),
 SCALE2(80),
 SCALE2(90),
 SCALE2(100),
 SCALE2(110),
 SCALE2(120),
};

void TimgFilterBlur::processTimesmooth(const unsigned char *srcY,const unsigned char *srcU,const unsigned char *srcV,unsigned char *dstY,unsigned char *dstU,unsigned char *dstV,const TfilterBlur *cfg)
{
 if (cfg->settings.tempSmooth!=oldTimesmoothStrength)
  {
   oldTimesmoothStrength=cfg->settings.tempSmooth;
   for (int i=0;i<=510;i++)
    {
     int sqerr=((i-255)*(i-255))>>oldTimesmoothStrength;
     if (sqerr>16) sqerr=16;
     sqerr=16-sqerr;
     square_table[i]=sqerr;
    } 
  }
 if (!accumY)
  {
   accumY=(unsigned char*)xvid_malloc(dxY *dyY *KERNEL,MCACHE_LINE);memset(accumY,  0,dxY *dyY *KERNEL);
   accumU=(unsigned char*)xvid_malloc(dxUV*dyUV*KERNEL,MCACHE_LINE);memset(accumU,128,dxUV*dyUV*KERNEL);
   accumV=(unsigned char*)xvid_malloc(dxUV*dyUV*KERNEL,MCACHE_LINE);memset(accumV,128,dxUV*dyUV*KERNEL);
  }
 timesmooth(srcY,dstY,accumY,dxY ,strideY ,dyY );dstY[0]=srcY[0];dstY[1]=srcY[1];
 timesmooth(srcU,dstU,accumU,dxUV,strideUV,dyUV);dstU[0]=srcU[0];dstU[1]=srcU[1];
 timesmooth(srcV,dstV,accumV,dxUV,strideUV,dyUV);dstV[0]=srcV[0];dstV[1]=srcV[1];
 if (++framecount>=KERNEL) framecount=0;
}

void TimgFilterBlur::timesmooth(const unsigned char *src,unsigned char *dst,unsigned char *Iaccum,unsigned int dx,unsigned int stride,unsigned int dy)
{
 int offset1=framecount;
 int offset2=((framecount+KERNEL-(KERNEL/2))%KERNEL);

 if (!framecount) 
  {
   unsigned char *accum=Iaccum;
   /*
   int w=dx,x;
   int y=dy;
   const unsigned char *srcdst=src;
   do
    {
     x=w;
     do
      {
       accum[0]=accum[1]=accum[2]=accum[3]=accum[4]=accum[5]=accum[6]=*srcdst++;             
      } while(--x);
     srcdst+=stride-dx;
    } while(--y);
   */
   accum[0]=accum[1]=accum[2]=accum[3]=accum[4]=accum[5]=accum[6]=src[stride*(dy-1)+(dx-1)];
  }
 unsigned int w=dx,x;
 unsigned int y=dy;
 unsigned char *accum=Iaccum;
 
 do
  {
   x=w;
   do
    {
     const unsigned char center=accum[offset2];
     const int *crtab=square_table+255-center;
     accum[offset1]=*src;src++;
     int raccum=0;
     int count=0;
     for(int i=0;i<KERNEL;i++)
      {
       const int c=*accum++;
       const int sqerr=crtab[c];
       raccum+=sqerr*c;
       count+=sqerr;
      }
     int divisor=((long*)scaletab)[2*count+1];
     raccum=(raccum*divisor)>>16;
     *dst++=raccum;
    } while(--x);
   src+=stride-dx;dst+=stride-dx;
  } while(--y);
}

void TimgFilterBlur::process(TffPict2 &pict,const Tfilter *cfg0)
{
 const TfilterBlur *cfg=(const TfilterBlur*)cfg0;
 if (!cfg->is) return;
 Trect *r=init(&pict.rect,cfg->full);
 if (cfg->settings.strength)
  {
   const unsigned char *srcY=getCurY(pict)+r->diffY;unsigned char *dstY=getNextY(pict)+r->diffY;
   processBlur(srcY,dstY,cfg);
  } 
 if (cfg->settings.tempSmooth) 
  {
   const unsigned char *srcY=getCurY(pict)+r->diffY ;unsigned char *dstY=getNextY(pict)+r->diffY ;
   const unsigned char *srcU=getCurU(pict)+r->diffUV;unsigned char *dstU=getNextU(pict)+r->diffUV;
   const unsigned char *srcV=getCurV(pict)+r->diffUV;unsigned char *dstV=getNextV(pict)+r->diffUV;
   processTimesmooth(srcY,srcU,srcV,dstY,dstU,dstV,cfg);
  }
}
