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
#include "TfilterPictprop.h"
#include "TimgFilterPictProp.h"
#include "CpictProp.h"

TimgFilterPictProp* TfilterPictProp::imgFilter=NULL;

const int TfilterPictProp::lumGainDef=128,TfilterPictProp::lumOffsetDef=0,TfilterPictProp::gammaCorrectionDef=100;
const int TfilterPictProp::hueDef=0,TfilterPictProp::saturationDef=64;

void TfilterPictProp::init(TcreateFcVector *cfcs,IffDecoder *deci)
{
 cfcs->push_back(&create);
 imgFilter=new TimgFilterPictProp;imgFilter->setDeci(deci);
}
Tfilter* TfilterPictProp::duplicate(void)
{
 TfilterPictProp *f=new TfilterPictProp;
 *f=*this;
 return f;
}
TimgFilter* TfilterPictProp::getImgFilter(void)
{
 return imgFilter;
}
TconfPage* TfilterPictProp::getConfPage(void)
{
 return new TpictPropPage;
}
