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

#pragma hdrstop
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "TimgFilterOffset.h"
#include "TpresetSettings.h"

TimgFilterOffset::TimgFilterOffset(void)
{
 old_offsetY_X=old_offsetY_Y=old_offsetU_X=old_offsetU_Y=old_offsetV_X=old_offsetV_Y=-12345;
}
void TimgFilterOffset::init(int Idx,int Istride,int Idy)
{
 TimgFilter::init(Idx,Istride,Idy);
}
void TimgFilterOffset::offset(const unsigned char *src,unsigned char *dst,int dx,int stride,int dy,int offsetX,int offsetY)
{
 int x1,x2;
 if (offsetX>0)
  {
   x1=offsetX;x2=0;
  }
 else 
  {
   offsetX=abs(offsetX);
   x1=0;x2=offsetX;
  } 
 int y1,y2; 
 if (offsetY>0)
  {
   y1=offsetY;y2=0;
  }
 else 
  {
   offsetY=abs(offsetY);
   y1=0;y2=offsetY;
  } 
 src+=y1*stride;dst+=y2*stride;
 for (int y=0;y<dy-offsetY;src+=stride,dst+=stride,y++)
  memcpy(dst+x1,src+x2,dx-offsetX);
}
void TimgFilterOffset::process(const unsigned char *srcY,const unsigned char *srcU,const unsigned char *srcV,
                               unsigned char *dstY,unsigned char *dstU,unsigned char *dstV,
                               const TpresetSettings *cfg)
{
 if (cfg->offsetY_X!=old_offsetY_X || cfg->offsetY_Y!=old_offsetY_Y)
  {
   old_offsetY_X=cfg->offsetY_X;old_offsetY_Y=cfg->offsetY_Y;
   for (unsigned char *dst=dstY,*dstEnd=dst+dyY*strideY;dst<dstEnd;dst+=strideY)
    memset(dst,0,dxY);
  };  
 offset(srcY,dstY,dxY,strideY,dyY,cfg->offsetY_X,cfg->offsetY_Y);

 if (cfg->offsetU_X!=old_offsetU_X || cfg->offsetU_Y!=old_offsetU_Y)
  {
   old_offsetU_X=cfg->offsetU_X;old_offsetU_Y=cfg->offsetU_Y;
   for (unsigned char *dst=dstU,*dstEnd=dst+dyUV*strideUV;dst<dstEnd;dst+=strideUV)
    memset(dst,128,dxUV);
  };  
 offset(srcU,dstU,dxUV,strideUV,dyUV,cfg->offsetU_X/2,-cfg->offsetU_Y/2);

 if (cfg->offsetV_X!=old_offsetV_X || cfg->offsetV_Y!=old_offsetV_Y)
  {
   old_offsetV_X=cfg->offsetV_X;old_offsetV_Y=cfg->offsetV_Y;
   for (unsigned char *dst=dstV,*dstEnd=dst+dyUV*strideUV;dst<dstEnd;dst+=strideUV)
    memset(dst,128,dxUV);
  };  
 offset(srcV,dstV,dxUV,strideUV,dyUV,cfg->offsetV_X/2,-cfg->offsetV_Y/2);
}
