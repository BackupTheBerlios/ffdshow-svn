#ifndef _TLIBAVCODEC_H_
#define _TLIBAVCODEC_H_

#include <assert.h>
#include "Tdll.h"

struct AVCodec;
struct AVCodecContext;
struct AVPicture;

class Tconfig;
class Tlibavcodec
{
private:
 Tdll *avcodec_dll;
 int* (*get_quant_store)(void);
public:
 Tlibavcodec()
  {
   ok=inited=false;
   avcodec_dll=NULL;
   quant_store=NULL;
  }
 void getVersion(char **vers); 
 bool ok,inited;
 void init(void);
 void done(void);
 void (*libavcodec_init)(void);
 AVCodec* (*avcodec_find_decoder_by_name)(const char *name);
 int  (*avcodec_open)(AVCodecContext *avctx, AVCodec *codec);
 int  (*avcodec_decode_video)(AVCodecContext *avctx, AVPicture *picture,
                              int *got_picture_ptr,
                              UINT8 *buf, int buf_size);
 void (*avcodec_flush_buffers)(AVCodecContext *avctx);
 int  (*avcodec_close)(AVCodecContext *avctx);
 void (*set_ff_idct)(void*);
 int *quant_store; 
};

#endif
