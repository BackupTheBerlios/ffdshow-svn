#ifndef _TRESIZECTX_H_
#define _TRESIZECTX_H_

#include "mplayer\libmpcodecs\img_format.h"
#include "Tpostproc.h"
#include "xvid\utils\mem_align.h"

struct SwsContext;
class TresizeCtx
{
private:
 int sizeY,sizeUV;
public:
 TresizeCtx(TpresetSettings *cfg)
  {
   isResize=cfg->isResize;
   swsc=NULL;
   imgResizeY=imgResizeU=imgResizeV=NULL;
  }
 void allocate(int IffDx,int IffDy)
  {
   FFdx=IffDx;FFdy=IffDy;
   strideY =(FFdx/16+4)*16;
   strideUV=strideY/2;
   imgResizeY=(unsigned char*)xvid_malloc(sizeY =strideY *(FFdy+8)  ,64);
   imgResizeU=(unsigned char*)xvid_malloc(sizeUV=strideUV*(FFdy+8)/2,64);
   imgResizeV=(unsigned char*)xvid_malloc(       strideUV*(FFdy+8)/2,64);
  } 
 ~TresizeCtx()
  {
   if (imgResizeY) xvid_free(imgResizeY);
   if (imgResizeU) xvid_free(imgResizeU);
   if (imgResizeV) xvid_free(imgResizeV);
  };
 inline void initResize(Tpostproc *postproc,int AVIdx,int AVIdy,TpresetSettings *cfg)
  {
   if (swsc) {postproc->freeSwsContext(swsc);swsc=NULL;};
   __asm emms;
   postproc->set_sws_params(cfg->resizeMethod,cfg->resizeGblurLum,cfg->resizeGblurChrom,cfg->resizeSharpenLum,cfg->resizeSharpenChrom);
   swsc=postproc->getSwsContextFromCmdLine(AVIdx,AVIdy,IMGFMT_YV12,imgDx,imgDy,IMGFMT_YV12);
  } 
 inline void clear(void)
  {
   memset(imgResizeY,0,sizeY);
   memset(imgResizeU,128,sizeUV);
   memset(imgResizeV,128,sizeUV);
  }
 inline void resize(unsigned char *srcY,unsigned char *srcU,unsigned char *srcV,int linesizeY,int linesizeU,int linesizeV,int height)
  {
   unsigned char *src[]={srcY,srcU,srcV};
   int srcStride[]={linesizeY,linesizeU,linesizeV};
   unsigned char *dst[]={imgResizeY+diffY*strideY+diffX,
                         imgResizeU+(diffY/2)*strideUV+diffX/2,
                         imgResizeV+(diffY/2)*strideUV+diffX/2};
   int dstStride[]={strideY,strideUV,strideUV};
   swsc->swScale(swsc,src,srcStride,0,height,dst,dstStride);
  }; 
 int isResize;    
 int FFdx,FFdy;
 int strideY,strideUV;
 int imgDx,imgDy;
 int diffX,diffY;
 unsigned char *imgResizeY,*imgResizeU,*imgResizeV; 
 SwsContext *swsc;
};

#endif
