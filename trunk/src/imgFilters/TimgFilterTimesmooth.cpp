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
#include "TimgFilterTimesmooth.h"
#include "TpresetSettings.h"
#include "xvid\utils\mem_align.h"
#include "ffdebug.h"

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

TimgFilterTimesmooth::TimgFilterTimesmooth(void)
{
 oldStrength=-1;
 framecount=0;
 accumY=accumU=accumV=NULL;
}
Trect* TimgFilterTimesmooth::init(TffRect *rect,int full)
{
 Trect *r=TimgFilter::init(rect,full);
 if (!accumY)
  {
   accumY=(unsigned char*)xvid_malloc(dxY *dyY *KERNEL,MCACHE_LINE);memset(accumY,  0,dxY *dyY *KERNEL);
   accumU=(unsigned char*)xvid_malloc(dxUV*dyUV*KERNEL,MCACHE_LINE);memset(accumU,128,dxUV*dyUV*KERNEL);
   accumV=(unsigned char*)xvid_malloc(dxUV*dyUV*KERNEL,MCACHE_LINE);memset(accumV,128,dxUV*dyUV*KERNEL);
  }
 return r;
}
void TimgFilterTimesmooth::done(void)
{
 if (accumY) xvid_free(accumY);accumY=NULL;
 if (accumU) xvid_free(accumU);accumU=NULL;
 if (accumV) xvid_free(accumV);accumV=NULL;
}
void TimgFilterTimesmooth::process(TffPict *pict,TffRect &rect,const TpresetSettings *cfg)
{
 if (!cfg->tempSmooth) return;
 Trect *r=init(&rect,cfg->fullBlur);
 const unsigned char *srcY=pict->getCurY()+r->diffY ;unsigned char *dstY=pict->getNextY()+r->diffY ;
 const unsigned char *srcU=pict->getCurU()+r->diffUV;unsigned char *dstU=pict->getNextU()+r->diffUV;
 const unsigned char *srcV=pict->getCurV()+r->diffUV;unsigned char *dstV=pict->getNextV()+r->diffUV;
 if (cfg->tempSmooth!=oldStrength)
  {
   oldStrength=cfg->tempSmooth;
   for (int i=0;i<=510;i++)
    {
     int sqerr=((i-255)*(i-255))>>oldStrength;
     if (sqerr>16) sqerr=16;
     sqerr=16-sqerr;
     square_table[i]=sqerr;
    } 
  }
 smooth(srcY,dstY,accumY,dxY ,strideY ,dyY );dstY[0]=srcY[0];dstY[1]=srcY[1];
 smooth(srcU,dstU,accumU,dxUV,strideUV,dyUV);dstU[0]=srcU[0];dstU[1]=srcU[1];
 smooth(srcV,dstV,accumV,dxUV,strideUV,dyUV);dstV[0]=srcV[0];dstV[1]=srcV[1];
 if (++framecount>=KERNEL) framecount=0;
}
void TimgFilterTimesmooth::smooth(const unsigned char *src,unsigned char *dst,unsigned char *Iaccum,unsigned int dx,unsigned int stride,unsigned int dy)
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
