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
#include "TimgFilterSubtitles.h"
#include "TpresetSettings.h"
#include "IffDecoder.h"

void TimgFilterSubtitles::process(TffPict *pict,TffRect &rect,const TpresetSettings *cfg)
{
 if (deci->getParam2(IDFF_fontChanged))
  {
   font.init(cfg);
   deci->putParam(IDFF_fontChanged,0);
  }
 subtitle *sub;
 deci->getSubtitle(&sub); 
 if (sub)
  { 
   Trect *r=init(&rect,0);
   unsigned char *dstY=pict->getCurNextY(rect.stride,r)+r->diffY ;
   unsigned char *dstU=pict->getCurNextU(rect.stride,r)+r->diffUV;
   unsigned char *dstV=pict->getCurNextV(rect.stride,r)+r->diffUV;
   font.print(dstY,dstU,dstV,dxY,strideY,dyY,sub,cfg->subPosX,cfg->subPosY);
  }
}                                  
