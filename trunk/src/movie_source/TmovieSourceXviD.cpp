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
#include <windows.h>
#include "Tdll.h"
#include "TmovieSourceXviD.h"
#include "xvid\xvid.h"
#include "ffmpeg\libavcodec\avcodec.h"

#define XVID_PATH "C:\\mydocuments\\ffdshow\\src\\ffmpeg\\xvid\\xvid.dll"

TmovieSourceXviD::TmovieSourceXviD(void)
{
 dll=NULL;
 dechandle=NULL;
}
bool TmovieSourceXviD::init(int codecId,unsigned int AVIdx,unsigned int AVIdy)
{
 if (codecId!=CODEC_ID_MPEG4) return false;
 dll=new Tdll(XVID_PATH);
 if (dll->ok)
  {
   dll->loadFunction((void**)&xvid_init,"xvid_init");
   dll->loadFunction((void**)&xvid_decore,"xvid_decore");
   XVID_INIT_PARAM xinit;
   xinit.cpu_flags=0;
   xvid_init(NULL,0,&xinit,NULL);
   XVID_DEC_PARAM xparam;
   xparam.width=AVIdx;
   xparam.height=AVIdy;
   xparam.handle=NULL;
   int xerr=xvid_decore(NULL,XVID_DEC_CREATE,&xparam,NULL);
   if (xerr!=XVID_ERR_OK) return false;
   dx=AVIdx;dy=AVIdy;
   stride=(dx/16+4)*16;
   dechandle=xparam.handle;
   return true;
  }
 else
  return false; 
}
void TmovieSourceXviD::done(void)
{
 if (dll)
  {
   if (dechandle) xvid_decore(dechandle,XVID_DEC_DESTROY,NULL,NULL);dechandle=NULL;
   delete dll;
   dll=NULL;
  }; 
 xvid_init=NULL;
 xvid_decore=NULL;
}
int TmovieSourceXviD::getFrame(const TglobalSettings *global,const TpresetSettings *cfg,const unsigned char *src,unsigned int srcLen, AVPicture *avpict,int &got_picture)
{
 XVID_DEC_FRAME xframe;
 xframe.bitstream=(void*)src;
 xframe.length=srcLen;
 struct DEC_PICTURE
  {
   void *y;
   void *u;
   void *v;
   int stride_y;
   int stride_uv;
  } xpict;
 xframe.image=&xpict;
 xframe.stride=stride;
 xframe.colorspace=XVID_CSP_USER;
 int xerr=xvid_decore(dechandle,XVID_DEC_DECODE,&xframe,NULL);
 if (xerr==XVID_ERR_OK)
  {
   got_picture=24;
   avpict->data[0]=(unsigned char*)xpict.y;avpict->data[1]=(unsigned char*)xpict.u;avpict->data[2]=(unsigned char*)xpict.v;
   avpict->linesize[0]=xpict.stride_y;avpict->linesize[1]=xpict.stride_uv;avpict->linesize[2]=xpict.stride_uv;
   for (unsigned int i=0;i<quantDx*quantDy;i++)
    quant[i]=10;
   return srcLen;
  }
 else
  {
   got_picture=0;
   avpict->data[0]=avpict->data[1]=avpict->data[2]=NULL;
   return 0;
  } 
}

bool TmovieSourceXviD::getVersion(char **vers)
{
 static char ver[1024];
 ver[0]='\0';
 if (vers) *vers=ver;
 Tdll *dl=new Tdll(XVID_PATH);
 int (*xvid_init)(void *handle, int opt, void *param1, void *param2);
 dl->loadFunction((void**)&xvid_init,"xvid_init");
 bool res=false;
 if (xvid_init) 
  {
   res=true;
   XVID_INIT_PARAM xinit;
   xinit.cpu_flags=0;
   xvid_init(NULL,0,&xinit,NULL);
   sprintf(ver,"XviD: API version %i.%i",xinit.api_version>>16,xinit.api_version&0xffff);
  };
 delete dl;  
 return res;
}
