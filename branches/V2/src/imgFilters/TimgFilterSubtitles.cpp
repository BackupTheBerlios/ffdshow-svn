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
#include "TimgFilterSubtitles.h"
#include "TfilterSubtitles.h"
#include "IffDecoder.h"
#include "subtitles\Tsubtitles.h"

TimgFilterSubtitles::TimgFilterSubtitles(void)
{
 settingsOld.size=-1;
 subs=new Tsubtitles;
 oldSubFlnm[0]='\0';
}

TimgFilterSubtitles::~TimgFilterSubtitles()
{
 if (subs) delete subs;
}

void TimgFilterSubtitles::process(TffPict2 &pict,const Tfilter *cfg0)
{
 const TfilterSubtitles *cfg=(const TfilterSubtitles*)cfg0;
 if (!cfg->is) return;
 
 if (cfg->settings.flnm[0]=='\0') return;
 if (strcmp(oldSubFlnm,cfg->settings.flnm)!=0)
  {
   strcpy(oldSubFlnm,cfg->settings.flnm);
   __asm emms;
   double AVIfps=deci->getAVIfps2()/1000.0;
   subs->init(NULL,oldSubFlnm,AVIfps);
  }
 
 int framesDelay;
 if (cfg->settings.delay)
  {
   __asm emms;
   double AVIfps=deci->getAVIfps2()/1000.0;
   framesDelay=(AVIfps==0)?cfg->settings.delay:cfg->settings.delay*AVIfps/1000;
  }
 else 
  framesDelay=0;
 int t1=deci->getCurrentFrame2(); 
 int sframe=1000*(int(t1)-framesDelay)/cfg->settings.speed;
 subtitle *sub=(sframe<1)?NULL:subs->getSubtitle(sframe);
 if (!sub) return;
 
 if (cfg->fontSettings.settings!=settingsOld)
  {
   settingsOld=cfg->fontSettings.settings;
   font.init(&cfg->fontSettings);
  }
 if (sub)
  { 
   Trect *r=init(&pict.rect,cfg->full);
   unsigned char *dstY=getCurNextY(pict)+r->diffY ;
   unsigned char *dstU=getCurNextU(pict)+r->diffUV;
   unsigned char *dstV=getCurNextV(pict)+r->diffUV;
   font.print(dstY,dstU,dstV,dxY,strideY,dyY,sub,cfg->settings.posX,cfg->settings.posY);
  }
}                                  
