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
#include "TmovieSourceLibavcodec.h"
#include "Tdll.h"
#include "ffdebug.h"
#include "TglobalSettings.h"
#include "TpresetSettings.h"
#include "xvid\dct\idct.h"
#include "ffmpeg\libavcodec\avcodec.h"

#define AVCODEC_PATH "C:\\mydocuments\\ffdshow\\src\\ffmpeg\\libavcodec.dll"

TmovieSourceLibavcodec::TmovieSourceLibavcodec(void)
{
 dll=NULL;avctx=NULL;
}
bool TmovieSourceLibavcodec::init(int codecId,unsigned int AVIdx,unsigned int AVIdy)
{
 if (!codecId || codecId>=CODEC_ID_YUY2) return false;
 dll=new Tdll(AVCODEC_PATH);
 dll->loadFunction((void**)&libavcodec_init,"libavcodec_init");
 dll->loadFunction((void**)&avcodec_find_decoder,"avcodec_find_decoder");
 dll->loadFunction((void**)&avcodec_open,"avcodec_open");
 dll->loadFunction((void**)&avcodec_decode_video,"avcodec_decode_video");
 dll->loadFunction((void**)&avcodec_flush_buffers,"avcodec_flush_buffers");
 dll->loadFunction((void**)&avcodec_close,"avcodec_close");
 dll->loadFunction((void**)&set_ff_idct,"set_ff_idct");
 if (dll->ok)
  {
   libavcodec_init();
   set_ff_idct((void*)1);
   avctx=(AVCodecContext*)malloc(sizeof(AVCodecContext));
   memset(avctx,0,sizeof(AVCodecContext));
   avctx->width =dx=AVIdx;
   avctx->height=dy=AVIdy;
   avctx->quant_store=quant;avctx->qstride=quantDx;
   DEBUGS("avcodec_find_decoder_by_name before");
   AVCodec *avcodec=avcodec_find_decoder(codecId);
   if (!avcodec) return false;
   DEBUGS("avcodec_find_decoder_by_name after");
   DEBUGS("avcodec_open before");
   if (avcodec_open(avctx,avcodec)<0) return false;
   DEBUGS("avcodec_open after");
   idctOld=-1;
   return true;
  }
 else return false;
}
void TmovieSourceLibavcodec::done(void)
{
 if (avctx)
  {
   avcodec_close(avctx);
   free(avctx);
   avctx=NULL;  
  }
 if (dll)
  {
   delete dll;
   dll=NULL;
  }
 libavcodec_init=NULL;
 avcodec_find_decoder=NULL;
 avcodec_open=NULL;
 avcodec_decode_video=NULL;
 avcodec_close=NULL;
 set_ff_idct=NULL;
}
TffPict2 TmovieSourceLibavcodec::getFrame(const TglobalSettings *global,const TpresetSettings *cfg,const unsigned char *src,unsigned int srcLen,int &used_bytes,int &got_picture)
{
 if (cfg->settings.idct!=idctOld)
  switch (idctOld=cfg->settings.idct)
   {  
    case 2:set_ff_idct(idct_ref);break;
    case 4:set_ff_idct(xvid_idct_ptr);break;
  //case 3:set_ff_idct((void*)3);break;
    case 1:set_ff_idct((void*)2);break;
    case 0:
    case 3:
    default:set_ff_idct((void*)1);break;
   }
 avctx->showMV=global->isShowMV;
 for (unsigned int i=0;i<quantDx*quantDy;i++)
  quant[i]=10;
 AVPicture avpict;
 used_bytes=avcodec_decode_video(avctx,&avpict,&got_picture,src,srcLen);
 return TffPict2(avpict.data[0],avpict.data[1],avpict.data[2],avpict.linesize[0],r,true);
}
TmovieSource::TmotionVectors TmovieSourceLibavcodec::getMV(void) const 
{
 TmotionVectors mv;
 mv.dx=avctx->mb_width;
 mv.dy=avctx->mb_height;
 mv.vectors=avctx->motion_vectors;
 return mv;
}

bool TmovieSourceLibavcodec::getVersion(char **vers)
{
 static char ver[1024];
 ver[0]='\0';
 if (vers) *vers=ver;
 Tdll *dl=new Tdll(AVCODEC_PATH);
 void (*av_getVersion)(char **version,char **build,char **datetime);
 dl->loadFunction((void**)&av_getVersion,"getVersion");
 bool res=false;
 if (av_getVersion) 
  {
   res=true;
   char *version,*build,*datetime;
   av_getVersion(&version,&build,&datetime);
   sprintf(ver,"libavcodec: version %s, build %s (%s)",version,build,datetime);
  }
 delete dl;  
 return res;
}
