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
#include "TimgFilters.h"
#include "TimgFilterPostproc.h"
#include "TimgFilterNoise.h"
#include "TimgFilterLuma.h"
#include "TimgFilterChroma.h"
#include "TimgFilterSharpen.h"
#include "TimgFilterSubtitles.h"
#include "TimgFilterBlur.h"
#include "TimgFilterOffset.h"
#include "TimgFilterTimesmooth.h"
#include "TimgFilterShowMV.h"
#include "TimgFilterResize.h"
#include "TglobalSettings.h"
#include "TpresetSettings.h"

using namespace std;

TimgFilters::TimgFilters(IffDecoder *deci)
{
 filters.push_back(postproc=new TimgFilterPostproc);postproc->setDeci(deci);
 filters.push_back(noise=new TimgFilterNoise);noise->setDeci(deci);
 filters.push_back(luma=new TimgFilterLuma);luma->setDeci(deci);
 filters.push_back(chroma=new TimgFilterChroma);chroma->setDeci(deci);
 filters.push_back(blur=new TimgFilterBlur);blur->setDeci(deci);
 filters.push_back(sharpen=new TimgFilterSharpen);sharpen->setDeci(deci);
 filters.push_back(subtitles=new TimgFilterSubtitles);subtitles->setDeci(deci);
 filters.push_back(offset=new TimgFilterOffset);offset->setDeci(deci);
 filters.push_back(timesmooth=new TimgFilterTimesmooth);timesmooth->setDeci(deci);
 filters.push_back(showMV=new TimgFilterShowMV);showMV->setDeci(deci);
 filters.push_back(resize=new TimgFilterResize);resize->setDeci(deci);
}
TimgFilters::~TimgFilters()
{
 for (vector<TimgFilter*>::iterator i=filters.begin();i!=filters.end();i++)
  {
   (*i)->done();
   delete *i;
  };
 filters.clear();
}
void TimgFilters::process(const TglobalSettings *global,const TpresetSettings *cfg,TffPict *pict,TffRect &rect)
{
 for (int i=cfg->min_order;i<=cfg->max_order;i++)
  if (i==cfg->orderPostproc && cfg->isPostproc)
   postproc->process(pict,rect,cfg);
  else if (i==cfg->orderPictProp && cfg->isPictProp)
   {
    luma->process(pict,rect,cfg);
    chroma->process(pict,rect,cfg);
   }
  else if (i==cfg->orderBlur && cfg->isBlur)
   {
    blur->process(pict,rect,cfg);
    timesmooth->process(pict,rect,cfg);
   }
  else if (i==cfg->orderSharpen && cfg->isSharpen)
   sharpen->process(pict,rect,cfg);
  else if (i==cfg->orderNoise && cfg->isNoise)
   noise->process(pict,rect,cfg);
  else if (i==cfg->orderSubtitles && cfg->isSubtitles)
   subtitles->process(pict,rect,cfg);
  else if (i==cfg->orderOffset && cfg->isOffset)
   offset->process(pict,rect,cfg);
  else if (i==cfg->orderResize && (cfg->isResize || cfg->resizeAspect!=0))
   resize->process(pict,rect,cfg);
  else if (i==cfg->orderShowMV && global->isShowMV) 
   showMV->process(pict,rect,cfg);
}
