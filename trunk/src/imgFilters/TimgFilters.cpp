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

#include <assert.h>
extern "C"
{
 #define FF_POSTPROCESS
 #include "ffmpeg\libavcodec\avcodec.h"
}
#include "TimgFilters.h"
#include "TpresetSettings.h"
#include "ffdebug.h"

TimgFilters::TimgFilters(TpresetSettings *Icfg):cfg(Icfg)
{
 cpus=cpu.GetCPUCount();
 postproc.init();
 tempY=tempU=tempV=NULL;
}
void TimgFilters::init(int IdxY,int IstrideY,int Idy,int IdiffX,int IdiffY)
{
 dxY =IdxY  ;strideY =IstrideY  ;
 dxUV=IdxY/2;strideUV=IstrideY/2;
 dy=Idy;
 diffY=IdiffY*strideY+IdiffX;
 diffUV=(IdiffY/2)*strideUV+(IdiffX/2);
 done();
 tempY=new TtempPicture(strideY*dy+16,0);
 tempU=new TtempPicture(strideUV*dy/2,128);
 tempV=new TtempPicture(strideUV*dy/2,128);
 IdxY-=IdiffX*2;_dy=Idy-=IdiffY*2;
 _dx=IdxY;_dy=Idy;
 noise.init(IdxY,IstrideY,Idy);
 luma.init(IdxY,IstrideY,Idy);
 chroma.init(IdxY,IstrideY,Idy);
 blur.init(IdxY,IstrideY,Idy);
 sharpen.init(IdxY,IstrideY,Idy);
 subtitles.init(IdxY,IstrideY,Idy);
}
void TimgFilters::done(void)
{
 if (tempY) {delete tempY;tempY=NULL;};
 if (tempU) {delete tempU;tempU=NULL;};
 if (tempV) {delete tempV;tempV=NULL;};
}
TimgFilters::~TimgFilters()
{
 done();
 postproc.done();
}
void TimgFilters::setSubtitle(subtitle *Isub)
{
 subtitles.sub=Isub;
}
void TimgFilters::process(unsigned char *srcY,unsigned char *srcU,unsigned char *srcV,unsigned char **dstY,unsigned char **dstU,unsigned char **dstV,int *quant_store)
{
 tempY->reset(srcY);tempU->reset(srcU);tempV->reset(srcV);
 for (int i=cfg->min_order;i<=cfg->max_order;i++)
  if (i==cfg->orderPostproc && postproc.ok && cfg->isPostproc)
   {
    if (cpus>0 && cfg->autoq && cfg->ppqual /*&& (avctx->frame_number%2)==0*/)
     {
      cpu.CollectCPUData();
      if (cpu.GetCPUUsage(0)>90)
       {
        if (cfg->currentq>0)
        cfg->currentq--;
       }
      else
       {
        if (cfg->currentq<cfg->ppqual)
         cfg->currentq++ ;
       };  
     };
    int ppmode=postproc.getPPmode(cfg);
    if (ppmode)
     {
      unsigned char *tempPict1[3]={
                                  tempY->getTempCur(),//+diffY,
                                  tempU->getTempCur(),//+diffUV,
                                  tempV->getTempCur(),//+diffUV
                                 },
                   *tempPict2[3]={
                                  tempY->getTempNext(),//+diffY,
                                  tempU->getTempNext(),//+diffUV,
                                  tempV->getTempNext(),//+diffUV
                                 };
      postproc.postprocess(tempPict1,strideY/*avpict.linesize[0]*/,
                           tempPict2,strideY/*avpict.linesize[0]*/,
                           dxY/*AVIdx*/,dy/*AVIdy*/,
                           quant_store,MBC+1,ppmode);
     }
   }
  else if (i==cfg->orderPictProp && cfg->isPictProp)
   {
    if (cfg->lumGain!=TpresetSettings::lumGainDef || cfg->lumOffset!=TpresetSettings::lumOffsetDef || cfg->gammaCorrection!=TpresetSettings::gammaCorrectionDef)
     {
      unsigned char *srcY=tempY->getTempCur()+diffY,*dstY=tempY->getTempNext()+diffY;
      luma.process(srcY,NULL,NULL,dstY,NULL,NULL,cfg);
     };
    if (cfg->hue!=TpresetSettings::hueDef || cfg->saturation!=TpresetSettings::saturationDef) 
     {
      unsigned char *srcU=tempU->getTempCur()+diffUV,*dstU=tempU->getTempNext()+diffUV;
      unsigned char *srcV=tempV->getTempCur()+diffUV,*dstV=tempV->getTempNext()+diffUV;
      chroma.process(NULL,srcU,srcV,NULL,dstU,dstV,cfg);
     }; 
   }
  else if (i==cfg->orderBlur && cfg->isBlur)
   {
    unsigned char *srcY=tempY->getTempCur()+diffY,*dstY=tempY->getTempNext()+diffY;
    blur.process(srcY,NULL,NULL,dstY,NULL,NULL,cfg);
   }
  else if (i==cfg->orderSharpen && cfg->isSharpen)
   {
    unsigned char *srcY=tempY->getTempCur()+diffY,*dstY=tempY->getTempNext()+diffY;
    sharpen.process(srcY,NULL,NULL,dstY,NULL,NULL,cfg);
   }
  else if (i==cfg->orderNoise && cfg->isNoise)
   {                           
    if (cfg->noiseStrength)
     {  
      unsigned char *srcY=tempY->getTempCur()+diffY,*dstY=tempY->getTempNext()+diffY;
      noise.process(srcY,NULL,NULL,dstY,NULL,NULL,cfg);
     }
    if (cfg->noiseStrengthChroma)
     {
      unsigned char *srcU=tempU->getTempCur()+diffUV,*dstU=tempU->getTempNext()+diffUV,
                    *srcV=tempV->getTempCur()+diffUV,*dstV=tempV->getTempNext()+diffUV;
      noise.process(NULL,srcU,srcV,NULL,dstU,dstV,cfg);
     } 
   } 
  else if (i==cfg->orderSubtitles && cfg->isSubtitles && subtitles.sub)
   {
    unsigned char *srcY=tempY->getTempCur()+diffY ,*dstY=tempY->getTempNext()+diffY ;
    unsigned char *srcU=tempU->getTempCur()+diffUV,*dstU=tempU->getTempNext()+diffUV;
    unsigned char *srcV=tempV->getTempCur()+diffUV,*dstV=tempV->getTempNext()+diffUV;
    subtitles.process(srcY,srcU,srcV,dstY,dstU,dstV,cfg);
   }
  
 *dstY=tempY->getTempCur();
 *dstU=tempU->getTempCur();
 *dstV=tempV->getTempCur();
}
