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
#include "TimgFilterCropNzoom.h"
#include "TfilterCropNzoom.h"
#include "IffDecoder.h"

TimgFilterCropNzoom::TimgFilterCropNzoom(void)
{
 settingsOld.magnificationX=-1;
 cropDx=-1;
}

void TimgFilterCropNzoom::calcCrop(const Trect *r,const TfilterCropNzoom *cfg)
{
 if (cfg->is)
  {
   cropDx=((100-cfg->settings.magnificationX)*r->dx)/100;
   cropDy=((100-cfg->settings.magnificationY)*r->dy)/100;
   cropLeft=(r->dx-cropDx)/2;
   cropTop =(r->dy-cropDy)/2;
  }
 else
  {
   cropDx=r->dx-(cfg->settings.cropLeft+cfg->settings.cropRight );
   cropDy=r->dy-(cfg->settings.cropTop +cfg->settings.cropBottom);
   cropLeft=cfg->settings.cropLeft;cropTop=cfg->settings.cropTop;
  }
 cropDx&=~7;cropDy&=~7;if (cropDx<=0) cropDx=8;if (cropDy<=0) cropDy=8;
 if (cropLeft+cropDx>=r->dx) cropLeft=r->dx-cropDx;if (cropTop+cropDy>=r->dy) cropTop=r->dy-cropDy;
 if (cropLeft>=r->dx-8) cropLeft=r->dx-8;if (cropTop>=r->dy-8) cropTop=r->dy-8;
}
void TimgFilterCropNzoom::done(void)
{
 cropDx=cropDy=-1;
}
void TimgFilterCropNzoom::process(TffPict2 &pict,const Tfilter *cfg0)
{
 const TfilterCropNzoom *cfg=(const TfilterCropNzoom*)cfg0;
 if (!cfg->is) return;
 Trect *r=init(&pict.rect,0);
 if (cropDx==-1 || cfg->settings!=settingsOld)
  {
   settingsOld=cfg->settings;
   calcCrop(r,cfg);
  }
 r->x=cropLeft;r->y=cropTop;r->dx=cropDx;r->dy=cropDy;r->calcDiff(pict.rect.stride);
}
