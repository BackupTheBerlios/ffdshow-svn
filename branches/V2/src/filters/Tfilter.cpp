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
#include "IffConstants.h"
#include "Tfilter.h"
#include <vector>
#include "TimgFilter.h"
#include "TfilterPostproc.h"
#include "TfilterNoise.h"
#include "TfilterPictProp.h"
#include "TfilterSharpen.h"
#include "TfilterSubtitles.h"
#include "TfilterBlur.h"
#include "TfilterOffset.h"
#include "TfilterShowMV.h"
#include "TfilterResizeNaspect.h"
#include "TfilterCropNzoom.h"

using namespace std;

void Tfilter::init(TcreateFcVector *cfcs,IffDecoder *deci)
{
 TfilterCropNzoom::init(cfcs,deci);
 TfilterPostproc::init(cfcs,deci);
 TfilterPictProp::init(cfcs,deci);
 TfilterNoise::init(cfcs,deci);
 TfilterBlur::init(cfcs,deci);
 TfilterSharpen::init(cfcs,deci);
 TfilterSubtitles::init(cfcs,deci);
 TfilterOffset::init(cfcs,deci);
 TfilterShowMV::init(cfcs,deci);
 TfilterResizeNaspect::init(cfcs,deci);
}
void Tfilter::done(void)
{
 delete TfilterCropNzoom::imgFilter;
 delete TfilterPostproc::imgFilter;
 delete TfilterPictProp::imgFilter;
 delete TfilterNoise::imgFilter;
 delete TfilterBlur::imgFilter;
 delete TfilterSharpen::imgFilter;
 delete TfilterSubtitles::imgFilter;
 delete TfilterOffset::imgFilter;
 delete TfilterShowMV::imgFilter;
 delete TfilterResizeNaspect::imgFilter;
}
Tfilter::Tparam Tfilter::getTparam0(unsigned int id)
{
 switch (id)
  {
   case IDFF_filterIs:return Tparam(&is);
   case IDFF_filterOrder:return Tparam(&order);
   case IDFF_filterFull:return Tparam(&full);
   default:return getTparam(id);
  }
}
bool Tfilter::getParam(unsigned int id,int &val)
{
 Tparam p=getTparam0(id);
 if (p.val)
  {
   val=*p.val;
   return true;
  } 
 else if (p.valC)
  {
   val=*p.valC;
   return true;
  }
 else   
  return false;
}
bool Tfilter::setParam(unsigned int id,int val)
{
 Tparam p=getTparam0(id);
 if (!p.val) return false;
 if (p.min || p.max)
  {
   if (val<p.min) val=p.min;
   if (val>p.max) val=p.max;
  }
 *p.val=val;
 return true;
}             
bool Tfilter::getParam(unsigned int id,char *val)
{
 Tparam p=getTparam0(id);
 if (!p.valS) return false;
 strcpy(val,p.valS);
 return true;
}
bool Tfilter::setParam(unsigned int id,char* val)
{
 Tparam p=getTparam0(id);
 if (!p.valS) return false;
 strcpy(p.valS,val);
 return true;
}
