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

TimgFilterResize::TimgFilterResize(void)
{
 swsc=NULL;
}
TffRect::Trect* TimgFilterResize::init(TffRect *rect,int full)
{
 TffRect::Trect *r=TimgFilter::init(rect,full);
 if (!swsc)
  {
   Tpostproc *postproc;deci->getPostproc(&postproc);
   postproc->set_sws_params(deci->getParam2(IDFF_resizeMethod),deci->getParam2(IDFF_resizeGblurLum),deci->getParam2(IDFF_resizeGblurChrom),deci->getParam2(IDFF_resizeSharpenLum),deci->getParam2(IDFF_resizeSharpenChrom));
   unsigned int outDx,outDy;deci->getOutputDimensions(&outDx,&outDy);
   swsc=postproc->getSwsContextFromCmdLine(r->dx,r->dy,IMGFMT_YV12,outDx,outDy,IMGFMT_YV12);
  }
 return r;
}
void TimgFilterResize::done(void)
{
 if (swsc) 
  {
   Tpostproc *postproc;deci->getPostproc(&postproc);
   postproc->freeSwsContext(swsc);
   swsc=NULL;
  };
}
void TimgFilterResize::process(TffPict *pict,TffRect &rect,const TpresetSettings *cfg)
{
 if (!cfg->blurStrength) return;
 TffRect::Trect *r=init(&rect,0);
 const unsigned char *srcY=pict->getCurY()+r->diffY ;unsigned char *dstY=pict->getNextY()+r->diffY ;
 const unsigned char *srcU=pict->getCurY()+r->diffUV;unsigned char *dstU=pict->getNextY()+r->diffUV;
 const unsigned char *srcV=pict->getCurY()+r->diffUV;unsigned char *dstV=pict->getNextY()+r->diffUV;
}
