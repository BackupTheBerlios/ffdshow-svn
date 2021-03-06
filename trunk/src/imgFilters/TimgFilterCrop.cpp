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
#include "TimgFilterCrop.h"
#include "TpresetSettings.h"
#include "IffDecoder.h"

TimgFilterCrop::TimgFilterCrop(void)
{
 oldSettings.magnificationX=-1;
 cropDx=-1;
}

void TimgFilterCrop::calcCrop(const Trect *r,const TpresetSettings *cfg)
{
 if (cfg->cropNzoom.isZoom)
  {
   int magX=cfg->cropNzoom.magnificationX;
   int magY=(cfg->cropNzoom.magnificationLocked)?cfg->cropNzoom.magnificationX:cfg->cropNzoom.magnificationY;
   cropDx=((100-magX)*r->dx)/100;
   cropDy=((100-magY)*r->dy)/100;
   cropLeft=(r->dx-cropDx)/2;
   cropTop =(r->dy-cropDy)/2;
  }
 else
  {
   cropDx=r->dx-(cfg->cropNzoom.cropLeft+cfg->cropNzoom.cropRight );
   cropDy=r->dy-(cfg->cropNzoom.cropTop +cfg->cropNzoom.cropBottom);
   cropLeft=cfg->cropNzoom.cropLeft;cropTop=cfg->cropNzoom.cropTop;
  }
 cropDx&=~7;cropDy&=~7;if (cropDx<=0) cropDx=8;if (cropDy<=0) cropDy=8;
 if (cropLeft+cropDx>=r->dx) cropLeft=r->dx-cropDx;if (cropTop+cropDy>=r->dy) cropTop=r->dy-cropDy;
 if (cropLeft>=r->dx-8) cropLeft=r->dx-8;if (cropTop>=r->dy-8) cropTop=r->dy-8;
}
void TimgFilterCrop::done(void)
{
 cropDx=cropDy=-1;
}
void TimgFilterCrop::process(TffPict2 &pict,const TpresetSettings *cfg)
{
 Trect *r=init(&pict.rect,0);
 if (cropDx==-1 || memcmp(&oldSettings,&cfg->cropNzoom,sizeof(TpresetSettings::TcropSettings))!=0)
  {
   oldSettings=cfg->cropNzoom;
   calcCrop(r,cfg);
  }
 r->x=cropLeft;r->y=cropTop;r->dx=cropDx;r->dy=cropDy;r->calcDiff(pict.rect.stride);
}
