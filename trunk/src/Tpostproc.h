#ifndef _TPOSTPROC_H_
#define _TPOSTPROC_H_

#include "Tdll.h"
#include "mplayer\cpudetect.h"
#include "mplayer\postproc\postprocess.h"
#ifndef uint8_t
typedef unsigned char uint8_t;
typedef short int int16_t;
typedef int int32_t;
#endif
#define __attribute__(x) 
#include "mplayer\postproc\swscale.h"
 
struct TpresetSettings;
class Tpostproc 
{
private:
 Tdll *postproc_dll;
public: 
 bool ok;      
 Tpostproc(void)
  {
   postproc_dll=NULL;
   ok=false;
  }
 void init(void);
 void done(void);
 void (*init_mplayer)(void);
 static int getPPmode(const TpresetSettings *cfg);
 void (*postprocess)(unsigned char * src[], int src_stride,
                     unsigned char * dst[], int dst_stride,
                     int horizontal_size,   int vertical_size,
                     QP_STORE_T *QP_store,  int QP_stride, int mode);
 SwsContext* (*getSwsContextFromCmdLine)(int srcW, int srcH, int srcFormat, int dstW, int dstH, int dstFormat);
 void (*freeSwsContext)(SwsContext *swsContext);
 void (*set_sws_params)(int Isws_flags,int Isws_lum_gblur,int Isws_chr_gblur,int Isws_lum_sharpen,int Isws_chr_sharpen);
 void (*yuy2toyv12)(const uint8_t *src, uint8_t *ydst, uint8_t *udst, uint8_t *vdst,
                    unsigned int width, unsigned int height,
                    unsigned int lumStride, unsigned int chromStride, unsigned int srcStride);
 void (*rgb24toyv12)(const uint8_t *src, uint8_t *ydst, uint8_t *udst, uint8_t *vdst,
                     unsigned int width, unsigned int height,
                     unsigned int lumStride, unsigned int chromStride, unsigned int srcStride);
private:
 int (*getPpModeForQuality)(int quality);
 void (*setModifyPPmode)(void (*mofidyPPmodeFnc)(PPMode *ppFnc,Tconfig *cfg),const Tconfig *cfg);
};

#endif
