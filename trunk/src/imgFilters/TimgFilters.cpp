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
#include "TglobalSettings.h"
#include "TpresetSettings.h"

using namespace std;

TimgFilters::TimgFilters(void)
{
 tempPict=NULL;
}
void TimgFilters::init(int IdxY,int IstrideY,int Idy,int dyFull,int IdiffX,int IdiffY,bool IafterResize)
{
 dxY =IdxY  ;strideY =IstrideY  ;
 dxUV=IdxY/2;strideUV=IstrideY/2;
 dy=Idy;
 afterResize=IafterResize;
 done();
 tempPict=new TtempPictures(strideY,dyFull,IdiffX,IdiffY);
 filters.push_back(&postproc);postproc.init(IdxY,IstrideY,Idy);
 filters.push_back(&noise);noise.init(IdxY,IstrideY,Idy);
 filters.push_back(&luma);luma.init(IdxY,IstrideY,Idy);
 filters.push_back(&chroma);chroma.init(IdxY,IstrideY,Idy);
 filters.push_back(&blur);blur.init(IdxY,IstrideY,Idy);
 filters.push_back(&sharpen);sharpen.init(IdxY,IstrideY,Idy);
 filters.push_back(&subtitles);subtitles.init(IdxY,IstrideY,Idy);
 filters.push_back(&offset);offset.init(IdxY,IstrideY,Idy);
 filters.push_back(&timesmooth);timesmooth.init(IdxY,IstrideY,Idy);
 filters.push_back(&showMV);showMV.init(IdxY,IstrideY,Idy);
}
void TimgFilters::done(void)
{
 if (tempPict) delete tempPict;tempPict=NULL;
 for (vector<TimgFilter*>::iterator i=filters.begin();i!=filters.end();i++)
  (*i)->done();
 filters.clear();
}
TimgFilters::~TimgFilters()
{
 done();
}
void TimgFilters::setSubtitle(subtitle *Isub)
{
 subtitles.sub=Isub;
}
void TimgFilters::process(TglobalSettings *global,TpresetSettings *cfg,TmovieSource *movie,Tpostproc *pp,unsigned char *srcY,unsigned char *srcU,unsigned char *srcV,unsigned char **dstY,unsigned char **dstU,unsigned char **dstV)
{
 tempPict->reset(srcY,srcU,srcV);
 for (int i=cfg->min_order;i<=cfg->max_order;i++)
  if (i==cfg->orderPostproc && cfg->isPostproc)
   postproc.process(tempPict,cfg,afterResize,movie,pp);
  else if (i==cfg->orderPictProp && cfg->isPictProp)
   {
    luma.process(tempPict,cfg);
    chroma.process(tempPict,cfg);
   }
  else if (i==cfg->orderBlur && cfg->isBlur)
   {
    blur.process(tempPict,cfg);
    timesmooth.process(tempPict,cfg);
   }
  else if (i==cfg->orderSharpen && cfg->isSharpen)
   sharpen.process(tempPict,cfg);
  else if (i==cfg->orderNoise && cfg->isNoise)
   noise.process(tempPict,cfg);
  else if (i==cfg->orderSubtitles && cfg->isSubtitles)
   {
    subtitles.process(tempPict,cfg);
    cfg->fontChanged=false;
   }
  else if (i==cfg->orderOffset && cfg->isOffset)
   offset.process(tempPict,cfg);
 if (global->showMV && !afterResize)
  showMV.process(tempPict,cfg,movie);
 *dstY=(unsigned char*)tempPict->getCurY();
 *dstU=(unsigned char*)tempPict->getCurU();
 *dstV=(unsigned char*)tempPict->getCurV();
}
