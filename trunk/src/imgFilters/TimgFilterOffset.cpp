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
#include <stdlib.h>
#include <math.h>
#include "TimgFilterOffset.h"
#include "TpresetSettings.h"

TimgFilterOffset::TimgFilterOffset(void)
{
}
void TimgFilterOffset::init(int Idx,int Istride,int Idy)
{
 TimgFilter::init(Idx,Istride,Idy);
}
void TimgFilterOffset::process(unsigned char *srcY,unsigned char *srcU,unsigned char *srcV,
                               unsigned char *dstY,unsigned char *dstU,unsigned char *dstV,
                               TpresetSettings *cfg)
{
}
