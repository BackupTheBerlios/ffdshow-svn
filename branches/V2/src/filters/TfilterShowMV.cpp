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
#include "TfilterShowMV.h"
#include "TimgFilterShowMV.h"
#include "CshowMV.h"

TimgFilterShowMV* TfilterShowMV::imgFilter=NULL;

void TfilterShowMV::init(TcreateFcVector *cfcs,IffDecoder *deci)
{
 cfcs->push_back(&create);
 imgFilter=new TimgFilterShowMV;imgFilter->setDeci(deci);
}
Tfilter* TfilterShowMV::duplicate(void)
{
 TfilterShowMV *f=new TfilterShowMV;
 *f=*this;
 return f;
}
TimgFilter* TfilterShowMV::getImgFilter(void)
{
 return imgFilter;
}
TconfPage* TfilterShowMV::getConfPage(void)
{
 return new TshowMVpage;
}
