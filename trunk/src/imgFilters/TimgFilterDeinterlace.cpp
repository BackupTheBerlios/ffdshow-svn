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
#include "TimgFilterDeinterlace.h"
#include "TpresetSettings.h"
#include "IffDecoder.h"
#include "Tpostproc.h"

const int TimgFilterDeinterlace::deintMethods[]=
{
 LINEAR_IPOL_DEINT_FILTER,
 LINEAR_BLEND_DEINT_FILTER,
 CUBIC_IPOL_DEINT_FILTER,
 CUBIC_BLEND_DEINT_FILTER,
 MEDIAN_DEINT_FILTER
};

void TimgFilterDeinterlace::process(TffPict2 &pict,const TpresetSettings *cfg)
{
 Tpostproc *postproc;deci->getPostproc(&postproc);if (!postproc->ok) return;
 Trect *r=init(&pict.rect,cfg->fullDeinterlace);
 const unsigned char *tempPict1[3]={getCurY (pict)+r->diffY,getCurU (pict)+r->diffUV,getCurV (pict)+r->diffUV};
 unsigned char       *tempPict2[3]={getNextY(pict)+r->diffY,getNextU(pict)+r->diffUV,getNextV(pict)+r->diffUV};
 postproc->postprocess(tempPict1,strideY,
                       tempPict2,strideY,
                       dxY,dyY,
                       NULL,0,deintMethods[cfg->deinterlace.method]);
}
