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

TimgFilterSubtitles::TimgFilterSubtitles(void)
{
 sub=NULL;
}

void TimgFilterSubtitles::process(TtempPictures *pict,const TpresetSettings *cfg)
{
 if (cfg->fontChanged)
  font.init(cfg);
 if (sub)
  { 
   const unsigned char *srcY=pict->getCurY();unsigned char *dstY=pict->getNextY();
   const unsigned char *srcU=pict->getCurU();unsigned char *dstU=pict->getNextU();
   const unsigned char *srcV=pict->getCurV();unsigned char *dstV=pict->getNextV();
   int y;
   for (y=0;y<dyY;y++)
    memcpy(dstY+y*strideY,srcY+y*strideY,dxY);
   for (y=0;y<dyUV;y++)
    {
     memcpy(dstU+y*strideUV,srcU+y*strideUV,dxUV);
     memcpy(dstV+y*strideUV,srcV+y*strideUV,dxUV);
    }; 
   font.print(dstY,dstU,dstV,dxY,strideY,dyY,sub,cfg->subPosX,cfg->subPosY);
  }; 
}                                  
