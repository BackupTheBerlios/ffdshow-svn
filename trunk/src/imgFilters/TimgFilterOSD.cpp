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
#include "TimgFilterOSD.h"
#include "TpresetSettings.h"
#include "IffDecoder.h"

TimgFilterOSD::TimgFilterOSD(void)
{
 oldSettings.weight=-1;
}
void TimgFilterOSD::process(TffPict2 &pict,const TpresetSettings *cfg)
{
 if (memcmp(&oldSettings,&cfg->fontOSD,sizeof(TfontSettings))!=0)
  {
   oldSettings=cfg->fontOSD;
   font.init(cfg->fontOSD);
  }
 Trect *r=init(&pict.rect,true);
 unsigned char *dstY=getCurNextY(pict)+r->diffY ;
 unsigned char *dstU=getCurNextU(pict)+r->diffUV;
 unsigned char *dstV=getCurNextV(pict)+r->diffUV;
 //font.print(dstY,dstU,dstV,dxY,strideY,dyY,sub,cfg->subtitles.posX,cfg->subtitles.posY);
}
