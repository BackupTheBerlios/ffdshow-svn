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
#include "TfilterSubtitles.h"
#include "TimgFilterSubtitles.h"
#include "Csubtitles.h"
#include "Cfont.h"

TimgFilterSubtitles* TfilterSubtitles::imgFilter=NULL;

const int TfilterSubtitles::delayDef=0,TfilterSubtitles::speedDef=1000;

void TfilterSubtitles::init(TcreateFcVector *cfcs,IffDecoder *deci)
{
 cfcs->push_back(&create);
 imgFilter=new TimgFilterSubtitles;imgFilter->setDeci(deci);
}
Tfilter* TfilterSubtitles::duplicate(void)
{
 TfilterSubtitles *f=new TfilterSubtitles;
 *f=*this;
 return f;
}
TimgFilter* TfilterSubtitles::getImgFilter(void)
{
 return imgFilter;
}
TconfPage* TfilterSubtitles::getConfPage(void)
{
 return new TsubtitlesPage;
}
TconfPage* TfilterSubtitles::getConfSubPage(void)
{
 return new TfontPage;
}
