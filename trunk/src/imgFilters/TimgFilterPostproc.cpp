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
#include "TimgFilterPostproc.h"
#include "TpresetSettings.h"
#include "Tpostproc.h"
#include "TmovieSource.h"
#include "IffDecoder.h"
#include "ffdebug.h"

const int TpresetSettings::TpostprocSettings::deblockStrengthDef=256;

const int TpresetSettings::TdeinterlaceSettings::methodDef=2;

void TimgFilterPostproc::process(TffPict2 &pict,const TpresetSettings *cfg)
{
 TmovieSource *movie;deci->getMovieSource(&movie);
 Tpostproc *postproc;deci->getPostproc(&postproc);
 if (!postproc->ok) return;
 int currentq=deci->getParam2(IDFF_currentq);
 if (cfg->postproc.autoq && cfg->postproc.qual)
  {
   if (deci->getCpuUsage2()>90)
    {
     if (currentq>0) currentq--;
    }
   else
    {
     if (currentq<cfg->postproc.qual) currentq++ ;
    }
   deci->putParam(IDFF_currentq,currentq); 
  }
 int ppmode=postproc->getPPmode(cfg,currentq);
 if (ppmode)
  {
   Trect *r=init(&pict.rect,cfg->fullPostproc);
   if (dxY<16 || dyY<16) return;
   const unsigned char *tempPict1[3]={getCurY (pict)+r->diffY,getCurU (pict)+r->diffUV,getCurV (pict)+r->diffUV};
   unsigned char       *tempPict2[3]={getNextY(pict)+r->diffY,getNextU(pict)+r->diffUV,getNextV(pict)+r->diffUV};
   if (cfg->postproc.deblockStrength!=cfg->postproc.deblockStrengthDef/* || afterResize*/)
    for (unsigned int i=0;i<movie->quantDx*movie->quantDy;i++)
     {
      int q=(/*((afterResize)?16:*/movie->quant[i]/*)*/*cfg->postproc.deblockStrength)/256;
      if (q<1) q=1;else if (q>31) q=31;
      movie->quant[i]=q;
     }
   postproc->postprocess(tempPict1,strideY,
                         tempPict2,strideY,
                         dxY,dyY,
                         movie->quant,movie->quantDx,ppmode);
  }
}
