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
#include "TcpuUsage.h"
#include "TimgFilterPostproc.h"
#include "TpresetSettings.h"
#include "Tpostproc.h"
#include "TmovieSource.h"
#include "IffDecoder.h"

const int TpresetSettings::deblockStrengthDef=256;

TimgFilterPostproc::TimgFilterPostproc(void)
{
 cpu=new TcpuUsage;
 cpus=cpu->GetCPUCount();
}
TimgFilterPostproc::~TimgFilterPostproc()
{
 delete cpu;
}
void TimgFilterPostproc::process(TffPict *pict,TffRect &rect,const TpresetSettings *cfg)
{
 TmovieSource *movie;deci->getMovieSource(&movie);
 Tpostproc *postproc;deci->getPostproc(&postproc);
 if (!postproc->ok || dxY<16 || dyY<16) return;
 int currentq=deci->getParam2(IDFF_currentq);
 if (cpus>0 && cfg->autoq && cfg->ppqual)
  {
   cpu->CollectCPUData();
   if (cpu->GetCPUUsage(0)>90)
    {
     if (currentq>0) currentq--;
    }
   else
    {
     if (currentq<cfg->ppqual) currentq++ ;
    };  
   deci->putParam(IDFF_currentq,currentq); 
  };
 int ppmode=postproc->getPPmode(cfg,currentq);
 if (ppmode)
  {
   Trect *r=init(&rect,0);
   const unsigned char *tempPict1[3]={pict->getCurY() +r->diffY,pict->getCurU() +r->diffUV,pict->getCurV() +r->diffUV};
   unsigned char       *tempPict2[3]={pict->getNextY()+r->diffY,pict->getNextU()+r->diffUV,pict->getNextV()+r->diffUV};
   if (cfg->deblockStrength!=TpresetSettings::deblockStrengthDef/* || afterResize*/)
    for (unsigned int i=0;i<movie->quantDx*movie->quantDy;i++)
     {
      int q=(/*((afterResize)?16:*/movie->quant[i]/*)*/*cfg->deblockStrength)/256;
      if (q<1) q=1;else if (q>31) q=31;
      movie->quant[i]=q;
     }
   postproc->postprocess(tempPict1,strideY,
                         tempPict2,strideY,
                         dxY,dyY,
                         movie->quant,movie->quantDx,ppmode);
  }
}
