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
#include "xvid\utils\mem_align.h"
#include "TmovieSourceUncompressed.h"
#include "xvid\image\image.h"
#include "xvid\xvid.h"
#include "ffmpeg\libavcodec\avcodec.h"

TmovieSourceUncompressed::TmovieSourceUncompressed(void)
{
 yuvY=yuvU=yuvV=NULL;
}
bool TmovieSourceUncompressed::init(int codecId,unsigned int AVIdx,unsigned int AVIdy)
{
 switch (codecId)
  {
   case CODEC_ID_YUY2:csp=XVID_CSP_YUY2;break;
   case CODEC_ID_RGB2:csp=XVID_CSP_RGB24;break;
   case CODEC_ID_RGB3:csp=XVID_CSP_RGB32;break;
   case CODEC_ID_RGB5:csp=XVID_CSP_RGB555;break;
   case CODEC_ID_RGB6:csp=XVID_CSP_RGB565;break;
   default:return false;
  }
 dx=AVIdx;dy=AVIdy;
 stride=(dx/16+4)*16;
 yuvY=(unsigned char*)xvid_malloc(stride*dy,MCACHE_LINE);
 yuvU=(unsigned char*)xvid_malloc((stride/2)*(dy/2),MCACHE_LINE);
 yuvV=(unsigned char*)xvid_malloc((stride/2)*(dy/2),MCACHE_LINE);
 return true;
}
void TmovieSourceUncompressed::done(void)
{
 if (yuvY) xvid_free(yuvY);yuvY=NULL;
 if (yuvU) xvid_free(yuvU);yuvU=NULL;
 if (yuvV) xvid_free(yuvV);yuvV=NULL;
}
TffPict2 TmovieSourceUncompressed::getFrame(const TglobalSettings *global,const TpresetSettings *cfg,const unsigned char *src,unsigned int srcLen,int &used_bytes,int &got_picture)
{
 IMAGE img={yuvY,yuvU,yuvV};
 image_input(&img,dx,dy,stride,src,csp);
 /*
 if (codecId==CODEC_ID_YUY2)
  postproc.yuy2toyv12((const unsigned char*)m_frame.bitstream,avpict.data[0],avpict.data[1],avpict.data[2],
                       avctx->width,avctx->height,
                       avpict.linesize[0],avpict.linesize[1],avctx->width*2);
 else if (codecId==CODEC_ID_RGB2)
  postproc.rgb24toyv12((const unsigned char*)m_frame.bitstream,avpict.data[0]+avpict.linesize[0]*(avctx->height-1),avpict.data[1]+avpict.linesize[1]*(avctx->height-2)/2,avpict.data[2]+avpict.linesize[2]*(avctx->height-2)/2,
                        avctx->width,avctx->height,
                        -avpict.linesize[0],-avpict.linesize[1],avctx->width*3);
 */                                     
 got_picture=24;used_bytes=srcLen; 
 for (unsigned int i=0;i<quantDx*quantDy;i++)
  quant[i]=10;
 return TffPict2(yuvY,yuvU,yuvV,stride,r,false);
}
void TmovieSourceUncompressed::getVersion(char **vers)
{
 if (!vers) return;
 static char ver[]="Uncompressed movie source 1.0";
 *vers=ver;
}

