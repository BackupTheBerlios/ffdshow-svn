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

#include <stdlib.h>
#include "TmovieSource.h"
#include "TmovieSourceLibavcodec.h"
#include "TmovieSourceUncompressed.h"
#include "TmovieSourceXvid.h"
#include "ffmpeg\libavcodec\avcodec.h"

#define TESTSOURCE(Tsource)            \
 movie=new Tsource;                    \
 if (movie->init(codecId,AVIdx,AVIdy)) \
  return movie;                        \
 else                                  \
  delete movie; 
 
TmovieSource* TmovieSource::initSource(int codecId,int AVIdx,int AVIdy)
{
 TmovieSource *movie;
 bool xvidfirst=false; 
 if (codecId>=CODEC_ID_XVID_MASK)
  {
   xvidfirst=true;
   codecId-=CODEC_ID_XVID_MASK;
  }; 
 if (xvidfirst)
  {
   TESTSOURCE(TmovieSourceXviD)
   TESTSOURCE(TmovieSourceLibavcodec)
  } 
 else
  {
   TESTSOURCE(TmovieSourceLibavcodec)
   TESTSOURCE(TmovieSourceXviD)
  };
 TESTSOURCE(TmovieSourceUncompressed)
 return NULL; 
}
TmovieSource::~TmovieSource()
{
 if (quant) free(quant);
}
void TmovieSource::initQuant(void)
{
 quantDx=dx/8+1;
 quantDy=dy/8+1;
 quant=(int*)malloc(quantDx*quantDy*sizeof(int));
 for (int i=0;i<quantDx*quantDy;i++)
  quant[i]=10;
}
