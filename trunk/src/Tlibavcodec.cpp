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

#include <windows.h>
#include "Tlibavcodec.h"
#include "TpresetSettings.h"

#define FF_POSTPROCESS 1
#include "ffmpeg\libavcodec\avcodec.h"

#define AVCODEC_PATH "C:\\mydocuments\\ffdshow\\src\\ffmpeg\\libavcodec\\libavcodec.dll"

const int TpresetSettings::idctDef=0;

void Tlibavcodec::init(void)
{
 inited=true;
 avcodec_dll=new Tdll(AVCODEC_PATH);
 avcodec_dll->loadFunction((void**)&libavcodec_init,"libavcodec_init");
 avcodec_dll->loadFunction((void**)&avcodec_find_decoder,"avcodec_find_decoder");
 avcodec_dll->loadFunction((void**)&avcodec_open,"avcodec_open");
 avcodec_dll->loadFunction((void**)&avcodec_decode_video,"avcodec_decode_video");
 avcodec_dll->loadFunction((void**)&avcodec_flush_buffers,"avcodec_flush_buffers");
 avcodec_dll->loadFunction((void**)&avcodec_close,"avcodec_close");
 avcodec_dll->loadFunction((void**)&get_quant_store,"get_quant_store");
 avcodec_dll->loadFunction((void**)&set_ff_idct,"set_ff_idct");
 if (avcodec_dll->ok)
  {
   libavcodec_init();
   quant_store=get_quant_store();
   set_ff_idct((void*)1);
   ok=true;
  }
 else
  ok=false; 
}
void Tlibavcodec::done(void)
{
 if (avcodec_dll)
  {
   delete avcodec_dll;
   avcodec_dll=NULL;
  }; 
 libavcodec_init=NULL;
 avcodec_find_decoder=NULL;
 avcodec_open=NULL;
 avcodec_decode_video=NULL;
 avcodec_close=NULL;
 get_quant_store=NULL;
 set_ff_idct=NULL;
 quant_store=NULL;
 ok=false;
}
void Tlibavcodec::getVersion(char **vers)
{
 if (!vers) return;
 static char ver[1024];
 ver[0]='\0';
 *vers=ver;
 void (*av_getVersion)(char **version,char **build,char **datetime);
 Tdll *dl=NULL;
 if (avcodec_dll)
  avcodec_dll->loadFunction((void**)&av_getVersion,"getVersion");
 else
  {  
   dl=new Tdll(AVCODEC_PATH);
   dl->loadFunction((void**)&av_getVersion,"getVersion");
  };
 if (av_getVersion) 
  {
   char *version,*build,*datetime;
   av_getVersion(&version,&build,&datetime);
   sprintf(ver,"libavcodec: version %s, build %s (%s)",version,build,datetime);
  };
 if (dl) delete dl;  
}
