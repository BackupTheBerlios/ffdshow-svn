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

void TimgFilters::init(int IdxY,int IstrideY,int Idy,int dyFull,int IdiffX,int IdiffY)
{
 dxY =IdxY  ;strideY =IstrideY  ;
 dxUV=IdxY/2;strideUV=IstrideY/2;
 dy=Idy;
 done();
 tempPict=new TtempPictures(strideY,dyFull,IdiffX,IdiffY);
 #define ADD_FILTER(f) \
  filters.push_back(&f);f.setDeci(deci);f.init(IdxY,IstrideY,Idy);
 ADD_FILTER(postproc);
 ADD_FILTER(noise);
 ADD_FILTER(luma);
 ADD_FILTER(chroma);
 ADD_FILTER(blur);
 ADD_FILTER(sharpen);
 ADD_FILTER(subtitles);
 ADD_FILTER(offset);
 ADD_FILTER(timesmooth);
 ADD_FILTER(showMV);
 #undef ADD_FILTER
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
void TimgFilters::process(const TglobalSettings *global,const TpresetSettings *cfg,unsigned char *srcY,unsigned char *srcU,unsigned char *srcV,unsigned char **dstY,unsigned char **dstU,unsigned char **dstV)
{
 tempPict->reset(srcY,srcU,srcV);
 for (int i=cfg->min_order;i<=cfg->max_order;i++)
  if (i==cfg->orderPostproc && cfg->isPostproc)
   postproc.process(tempPict,cfg);
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
   subtitles.process(tempPict,cfg);
  else if (i==cfg->orderOffset && cfg->isOffset)
   offset.process(tempPict,cfg);
 if (global->showMV)
  showMV.process(tempPict,cfg);
 *dstY=(unsigned char*)tempPict->getCurY();
 *dstU=(unsigned char*)tempPict->getCurU();
 *dstV=(unsigned char*)tempPict->getCurV();
}
