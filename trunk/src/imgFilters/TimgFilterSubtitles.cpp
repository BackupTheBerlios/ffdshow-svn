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
#include "TpresetSettings.h"
#include "IffDecoder.h"

const int TpresetSettings::TsubtitlesSettings::delayDef=0,TpresetSettings::TsubtitlesSettings::speedDef=1000;

TimgFilterSubtitles::TimgFilterSubtitles(void)
{
 oldSettings.weight=-1;
}

void TimgFilterSubtitles::process(TffPict2 &pict,const TpresetSettings *cfg)
{
 subtitle *sub;
 deci->getSubtitle(&sub); 
 if (sub)
  {
   if (memcmp(&oldSettings,&cfg->font,sizeof(TfontSettings))!=0)
    {
     oldSettings=cfg->font;
     font.init(cfg->font);
    }
   Trect *r=init(&pict.rect,cfg->fullSubtitles);
   unsigned char *dstY=getCurNextY(pict)+r->diffY ;
   unsigned char *dstU=getCurNextU(pict)+r->diffUV;
   unsigned char *dstV=getCurNextV(pict)+r->diffUV;
   font.print(dstY,dstU,dstV,dxY,strideY,dyY,sub,cfg->subtitles.posX,cfg->subtitles.posY);
  }
}                                  
