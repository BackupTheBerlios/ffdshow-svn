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
#include "TcpuUsage.h"
#include "TimgFilterPostproc.h"
#include "TfilterPostproc.h"
#include "Tpostproc.h"
#include "TmovieSource.h"
#include "IffDecoder.h"
#include "ffdebug.h"

TimgFilterPostproc::TimgFilterPostproc(void)
{
 cpu=new TcpuUsage;
 cpus=cpu->GetCPUCount();
}
TimgFilterPostproc::~TimgFilterPostproc()
{
 delete cpu;
}
void TimgFilterPostproc::process(TffPict2 &pict,const Tfilter *cfg0)
{
 const TfilterPostproc *cfg=(const TfilterPostproc*)cfg0;
 if (!cfg->is) return;
 TmovieSource *movie;deci->getMovieSource(&movie);
 Tpostproc *postproc;deci->getPostproc(&postproc);
 if (!postproc->ok) return;
 int currentq=deci->getParam2(IDFF_currentq);
 if (cpus>0 && cfg->settings.autoq && cfg->settings.qual)
  {
   cpu->CollectCPUData();
   if (cpu->GetCPUUsage(0)>90)
    {
     if (currentq>0) currentq--;
    }
   else
    {
     if (currentq<cfg->settings.qual) currentq++ ;
    }
   deci->putParam(IDFF_currentq,currentq); 
  }
 int ppmode=postproc->getPPmode(cfg,currentq);
 if (ppmode)
  {
   Trect *r=init(&pict.rect,cfg->full);
   if (dxY<16 || dyY<16) return;
   const unsigned char *tempPict1[3]={getCurY (pict)+r->diffY,getCurU (pict)+r->diffUV,getCurV (pict)+r->diffUV};
   unsigned char       *tempPict2[3]={getNextY(pict)+r->diffY,getNextU(pict)+r->diffUV,getNextV(pict)+r->diffUV};
   if (cfg->settings.deblockStrength!=cfg->deblockStrengthDef/* || afterResize*/)
    for (unsigned int i=0;i<movie->quantDx*movie->quantDy;i++)
     {
      int q=(/*((afterResize)?16:*/movie->quant[i]/*)*/*cfg->settings.deblockStrength)/256;
      if (q<1) q=1;else if (q>31) q=31;
      movie->quant[i]=q;
     }
   postproc->postprocess(tempPict1,strideY,
                         tempPict2,strideY,
                         dxY,dyY,
                         movie->quant,movie->quantDx,ppmode);
  }
}
