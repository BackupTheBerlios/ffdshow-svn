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
#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include "TimgFilterResize.h"
#include "TpresetSettings.h"
#include "IffDecoder.h"
#include "Tpostproc.h"
#include "mplayer\libmpcodecs\img_format.h"

const TpresetSettings::resizeMethodNone=6;

TimgFilterResize::TimgFilterResize(void)
{
 swsc=NULL;
 resizeMethodOld=resizeGblurLumOld=resizeGblurChromOld=resizeSharpenLumOld=resizeSharpenChromOld=-1;
}
void TimgFilterResize::done(void)
{
 if (swsc) 
  {
   Tpostproc *postproc;deci->getPostproc(&postproc);
   if (postproc->ok) postproc->freeSwsContext(swsc);
   swsc=NULL;
  }
}
Trect TimgFilterResize::calcNewClip(const TpresetSettings *cfg,const Trect &oldClip,const Trect &newFull)
{
 Trect newClip;
 if (cfg->resizeAspect==0)
  newClip=newFull;
 else
  {
   int ax,ay;
   if (cfg->resizeAspect==1)
    {
     ax=oldClip.dx;
     ay=oldClip.dy;
    }
   else
    {
     ax=cfg->aspectRatio;
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
void TimgFilterResize::process(TffPict *pict,TffRect &rect,const TpresetSettings *cfg)
{
 Trect *r=init(&rect,0);
 if (r->dx==cfg->resizeDx && r->dy==cfg->resizeDy && cfg->resizeAspect!=2) return;
 if (!swsc || resizeMethodOld!=cfg->resizeMethod || resizeGblurLumOld!=cfg->resizeGblurLum || resizeGblurChromOld!=cfg->resizeGblurChrom || resizeSharpenLumOld!=cfg->resizeSharpenLum || resizeSharpenChromOld!=cfg->resizeSharpenChrom)
  {
   resizeMethodOld=cfg->resizeMethod;resizeGblurLumOld=cfg->resizeGblurLum;resizeGblurChromOld=cfg->resizeGblurChrom;resizeSharpenLumOld=cfg->resizeSharpenLum || resizeSharpenChromOld!=cfg->resizeSharpenChrom;
   done();
   Tpostproc *postproc;deci->getPostproc(&postproc);if (!postproc->ok) return;
   newRect.stride=(cfg->resizeDx/16+2)*16;
   newRect.full=Trect(0,0,cfg->resizeDx,cfg->resizeDy,newRect.stride);
   if (cfg->resizeMethod!=cfg->resizeMethodNone)
    {
     newRect.clip=calcNewClip(cfg,*r,newRect.full);newRect.clip.calcDiff(newRect.stride);
     __asm emms;
     swsc=postproc->getSwsContextFromCmdLine(r->dx,r->dy,IMGFMT_YV12,newRect.clip.dx,newRect.clip.dy,IMGFMT_YV12,resizeMethodOld,resizeGblurLumOld,resizeGblurChromOld,resizeSharpenLumOld,resizeSharpenChromOld);
    }
   else 
    {
     newRect.clip=newRect.full;newRect.clip.calcDiff(newRect.stride);
     swsc=(SwsContext*)-1;
    }
  }
 if (rect.clip==newRect.clip && rect.full==newRect.full) return;

 const unsigned char *src[]={pict->getCurY()+r->diffY ,pict->getCurU()+r->diffUV,pict->getCurV()+r->diffUV};
 int srcStride[]={rect.stride,rect.stride/2,rect.stride/2};
 unsigned char *y2=pict->getNextY(),*u2=pict->getNextU(),*v2=pict->getNextV();
 unsigned char *dst[]={y2+newRect.clip.diffY,u2+newRect.clip.diffUV,v2+newRect.clip.diffUV};
 int dstStride[]={newRect.stride,newRect.stride/2,newRect.stride/2};
 newRect.clear(y2+newRect.full.diffY,u2+newRect.full.diffUV,v2+newRect.full.diffUV);
 if (cfg->resizeAspect!=cfg->resizeMethodNone)
  swsc->swScale(swsc,src,srcStride,0,r->dy,dst,dstStride);
 else
  {
  }
 rect=newRect;
}
