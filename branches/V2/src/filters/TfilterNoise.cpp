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
#include "TfilterNoise.h"
#include "TimgFilterNoise.h"
#include "Cnoise.h"

TimgFilterNoise* TfilterNoise::imgFilter=NULL;

const int TfilterNoise::strengthDef=15,TfilterNoise::strengthChromaDef=0;

void TfilterNoise::init(TcreateFcVector *cfcs,IffDecoder *deci)
{
 cfcs->push_back(&create);
 imgFilter=new TimgFilterNoise;imgFilter->setDeci(deci);
}
Tfilter* TfilterNoise::duplicate(void)
{
 TfilterNoise *f=new TfilterNoise;
 *f=*this;
 return f;
}
TimgFilter* TfilterNoise::getImgFilter(void)
{
 return imgFilter;
}
TconfPage* TfilterNoise::getConfPage(void)
{
 return new TnoisePage;
}
