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
#include "TimgFilterOffset.h"
#include "TpresetSettings.h"
#include "IffDecoder.h"

void TimgFilterOffset::offset(const unsigned char *src,unsigned char *dst,unsigned int dx,unsigned int stride,unsigned int dy,int offsetX,int offsetY,unsigned char c)
{
 unsigned int x1,x2;
 if (offsetX>0)
  {
   x1=offsetX;x2=0;
  }
 else
  {
   offsetX=abs(offsetX);
   x1=0;x2=offsetX;
  }
 unsigned int y1,y2;
 if (offsetY>0)
  {
   y1=offsetY;y2=0;
  }
 else
  {
   offsetY=abs(offsetY);
   y1=0;y2=offsetY;
  }
 for (unsigned char *dstCx=dst+(x2?dx-offsetX:0),*dstCXend=dstCx+dy*stride;dstCx<dstCXend;dstCx+=stride)
  memset(dstCx,c,offsetX);
 for (unsigned char *dstCy=dst+(y1?dy-offsetY:0)*stride,*dstCYend=dstCy+offsetY*stride;dstCy<dstCYend;dstCy+=stride)
  memset(dstCy,c,dx);
 src+=y1*stride;dst+=y2*stride;
 for (unsigned int y=0;y<dy-offsetY;src+=stride,dst+=stride,y++)
  memcpy(dst+x1,src+x2,dx-offsetX);
}
void TimgFilterOffset::process(TffPict *pict,TffRect &rect,const TpresetSettings *cfg)
{
 if (!cfg->offsetY_X && !cfg->offsetY_Y && !cfg->offsetU_X && !cfg->offsetU_Y && !cfg->offsetV_X && !cfg->offsetV_Y) return;
 Trect *r=init(&rect,cfg->fullOffset);
 if (cfg->offsetY_X || cfg->offsetY_Y)
  {
   const unsigned char *srcY=pict->getCurY()+r->diffY;unsigned char *dstY=pict->getNextY()+r->diffY;
   offset(srcY,dstY,dxY,strideY,dyY,cfg->offsetY_X,-cfg->offsetY_Y,0);
  }
 if (cfg->offsetU_X || cfg->offsetU_Y)
  {
   const unsigned char *srcU=pict->getCurU()+r->diffUV;unsigned char *dstU=pict->getNextU()+r->diffUV;
   offset(srcU,dstU,dxUV,strideUV,dyUV,cfg->offsetU_X/2,-cfg->offsetU_Y/2,128);
  }
 if (cfg->offsetV_X || cfg->offsetV_Y)
  {
   const unsigned char *srcV=pict->getCurV()+r->diffUV;unsigned char *dstV=pict->getNextV()+r->diffUV;
   offset(srcV,dstV,dxUV,strideUV,dyUV,cfg->offsetV_X/2,-cfg->offsetV_Y/2,128);
  }
}
