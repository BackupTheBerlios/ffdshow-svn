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
#include "TimgFilterResizeNaspect.h"
#include "TfilterResizeNaspect.h"
#include "IffDecoder.h"
#include "Tpostproc.h"
#include "mplayer\libmpcodecs\img_format.h"

TimgFilterResizeNaspect::TimgFilterResizeNaspect(void)
{
 oldSettings.method=-1;
 swsc=NULL;
}
void TimgFilterResizeNaspect::done(void)
{
 if (swsc && swsc!=(void*)-1) 
  {
   Tpostproc *postproc;deci->getPostproc(&postproc);
   if (postproc->ok) postproc->freeSwsContext(swsc);
   swsc=NULL;
  }
}
Trect TimgFilterResizeNaspect::calcNewClip(const TfilterResizeNaspect *cfg,const Trect &oldClip,const Trect &newFull)
{
 Trect newClip;
 if (cfg->settings.isAspect==0)
  newClip=newFull;
 else
  {
   int ax,ay;
   if (cfg->settings.isAspect==1)
    {
     ax=oldClip.dx;
     ay=oldClip.dy;
    }
   else
    {
     ax=cfg->settings.aspectRatio;
     ay=1<<16;
    }
   newClip.dx=newFull.dx;
   newClip.dy=newFull.dx*ay/ax;
   if (newClip.dy>newFull.dy)
    {
     newClip.dx=newFull.dy*ax/ay;
     newClip.dy=newFull.dy;
    }
   newClip.dx&=~7;newClip.dy&=~7;
   newClip.x=(newFull.dx-newClip.dx)/2;newClip.x&=~3;
   newClip.y=(newFull.dy-newClip.dy)/2;newClip.y&=~3;
  }
 return newClip;
}
void TimgFilterResizeNaspect::process(TffPict2 &pict,const Tfilter *cfg0)
{
 const TfilterResizeNaspect *cfg=(const TfilterResizeNaspect*)cfg0;
 Trect *r=init(&pict.rect,cfg->full);
 if (r->dx==cfg->settings.dx && r->dy==cfg->settings.dy && cfg->settings.isAspect!=2) return;
 if (!swsc || cfg->settings!=oldSettings)
  {
   oldSettings=cfg->settings;
   done();
   Tpostproc *postproc;deci->getPostproc(&postproc);if (!postproc->ok) return;
   newRect.stride=(cfg->settings.dx/16+2)*16;
   newRect.full=Trect(0,0,cfg->settings.dx,cfg->settings.dy,newRect.stride);
   if (cfg->settings.method!=TfilterResizeNaspect::methodNone)
    {
     newRect.clip=calcNewClip(cfg,*r,newRect.full);newRect.clip.calcDiff(newRect.stride);
     __asm emms;
     swsc=postproc->getSwsContextFromCmdLine(r->dx,r->dy,IMGFMT_YV12,newRect.clip.dx,newRect.clip.dy,IMGFMT_YV12,cfg->settings.method,cfg->settings.GblurLum,cfg->settings.GblurChrom,cfg->settings.sharpenLum,cfg->settings.sharpenChrom);
    }
   else 
    {
     //newRect.clip=newRect.full;
     newRect.clip.dx=min(r->dx,newRect.full.dx);newRect.clip.x=(newRect.full.dx-newRect.clip.dx)/2;
     newRect.clip.dy=min(r->dy,newRect.full.dy);newRect.clip.y=(newRect.full.dy-newRect.clip.dy)/2;
     newRect.clip.calcDiff(newRect.stride);
     swsc=(SwsContext*)-1;
    }
  }
 if (pict.rect.clip==newRect.clip && pict.rect.full==newRect.full) return;

 const unsigned char *src[]={getCurY(pict)+r->diffY,getCurU(pict)+r->diffUV,getCurV(pict)+r->diffUV};
 int srcStride[]={pict.rect.stride,pict.rect.stride/2,pict.rect.stride/2};
 unsigned char *y2=getNextY(pict,&newRect),*u2=getNextU(pict,&newRect),*v2=getNextV(pict,&newRect);
 unsigned char *dst[]={y2+newRect.clip.diffY,u2+newRect.clip.diffUV,v2+newRect.clip.diffUV};
 int dstStride[]={newRect.stride,newRect.stride/2,newRect.stride/2};
 newRect.clear(y2+newRect.full.diffY,u2+newRect.full.diffUV,v2+newRect.full.diffUV);
 if (swsc && swsc!=(void*)-1)
  swsc->swScale(swsc,src,srcStride,0,r->dy,dst,dstStride);
 else
  {
   unsigned int dy,ydif1,ydif2;
   if (r->dy<newRect.clip.dy)
    {
     dy=r->dy;
     ydif1=(newRect.clip.dy-r->dy)/2;
     ydif2=0;
    }
   else
    {
     dy=newRect.clip.dy;
     ydif1=0;
     ydif2=(r->dy-newRect.clip.dy)/2;
    }
   unsigned int dx,xdif1,xdif2;
   if (r->dx<newRect.clip.dx)
    {
     dx=r->dx;
     xdif1=(newRect.clip.dx-r->dx)/2;
     xdif2=0;
    }
   else
    {
     dx=newRect.clip.dx;
     xdif1=0;
     xdif2=(r->dx-newRect.clip.dx)/2;
    }
   src[0]+=ydif2*pict.rect.stride;dst[0]+=ydif1*newRect.stride;
   unsigned int y;
   for (y=0;y<dy;y++,src[0]+=pict.rect.stride,dst[0]+=newRect.stride)
    memcpy(dst[0]+xdif1,src[0]+xdif2,dx);
   dx/=2;xdif1/=2;xdif2/=2;
   dy/=2;ydif1/=2;ydif2/=2;
   src[1]+=ydif2*pict.rect.stride/2;dst[1]+=ydif1*newRect.stride/2;
   src[2]+=ydif2*pict.rect.stride/2;dst[2]+=ydif1*newRect.stride/2;
   for (y=0;y<dy;y++,src[1]+=pict.rect.stride/2,src[2]+=pict.rect.stride/2,dst[1]+=newRect.stride/2,dst[2]+=newRect.stride/2)
    {
     memcpy(dst[1]+xdif1,src[1]+xdif2,dx);
     memcpy(dst[2]+xdif1,src[2]+xdif2,dx);
    }
  }
 pict.rect=newRect;
}
