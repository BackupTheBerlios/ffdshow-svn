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
#include "TfilterOffset.h"
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
void TimgFilterOffset::process(TffPict2 &pict,const Tfilter *cfg0)
{
 const TfilterOffset *cfg=(const TfilterOffset*)cfg0;
 if (!cfg->is) return;
 if (!cfg->settings.Y_X && !cfg->settings.Y_Y && !cfg->settings.U_X && !cfg->settings.U_Y && !cfg->settings.V_X && !cfg->settings.V_Y) return;
 Trect *r=init(&pict.rect,cfg->full);
 if (cfg->settings.Y_X || cfg->settings.Y_Y)
  {
   const unsigned char *srcY=getCurY(pict)+r->diffY;unsigned char *dstY=getNextY(pict)+r->diffY;
   offset(srcY,dstY,dxY,strideY,dyY,cfg->settings.Y_X,-cfg->settings.Y_Y,0);
  }
 if (cfg->settings.U_X || cfg->settings.U_Y)
  {
   const unsigned char *srcU=getCurU(pict)+r->diffUV;unsigned char *dstU=getNextU(pict)+r->diffUV;
   offset(srcU,dstU,dxUV,strideUV,dyUV,cfg->settings.U_X/2,-cfg->settings.U_Y/2,128);
  }
 if (cfg->settings.V_X || cfg->settings.V_Y)
  {
   const unsigned char *srcV=getCurV(pict)+r->diffUV;unsigned char *dstV=getNextV(pict)+r->diffUV;
   offset(srcV,dstV,dxUV,strideUV,dyUV,cfg->settings.V_X/2,-cfg->settings.V_Y/2,128);
  }
}
