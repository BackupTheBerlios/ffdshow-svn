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
#include "TfilterSharpen.h"
#include "TimgFilterSharpen.h"
#include "Csharpen.h"

TimgFilterSharpen* TfilterSharpen::imgFilter=NULL;

const int TfilterSharpen::xsharpStrengthDef =20,TfilterSharpen::xsharpThresholdDef=150;
const int TfilterSharpen::unsharpStrengthDef=40,TfilterSharpen::unsharpThresholdDef=0;

void TfilterSharpen::init(TcreateFcVector *cfcs,IffDecoder *deci)
{
 cfcs->push_back(&create);
 imgFilter=new TimgFilterSharpen;imgFilter->setDeci(deci);
}
Tfilter* TfilterSharpen::duplicate(void)
{
 TfilterSharpen *f=new TfilterSharpen;
 *f=*this;
 return f;
}
TimgFilter* TfilterSharpen::getImgFilter(void)
{
 return imgFilter;
}
TconfPage* TfilterSharpen::getConfPage(void)
{
 return new TsharpenPage;
}
