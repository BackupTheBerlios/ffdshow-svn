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

#include <stddef.h>
#include "TmovieSource.h"
#include "TmovieSourceLibavcodec.h"
#include "TmovieSourceUncompressed.h"
#include "TmovieSourceXvid.h"

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
/*  
 movie=new TmovieSourceLibavcodec;
 if (movie->init(codecId,AVIdx,AVIdy))
  return movie;
 else 
  delete movie; 
 movie=new TmovieSourceUncompressed;
 if (movie->init(codecId,AVIdx,AVIdy))
  return movie;
 else
  delete movie; 
 movie=new TmovieSourceXviD;
 if (movie->init(codecId,AVIdx,AVIdy))
  return movie;
 else
  delete movie; 
*/  
 return NULL; 
}
