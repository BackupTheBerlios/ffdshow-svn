#ifndef _TMOVIESOURCELIBAVCODEC_H_
#define _TMOVIESOURCELIBAVCODEC_H_

#include "TmovieSource.h"

class Tdll;
struct AVCodecContext;
class TmovieSourceLibavcodec :public TmovieSource
{
private:
 Tdll *dll;
 int* (*get_quant_store)(void);
 void (*libavcodec_init)(void);
 AVCodec* (*avcodec_find_decoder)(int codecId);
 int  (*avcodec_open)(AVCodecContext *avctx, AVCodec *codec);
 int  (*avcodec_decode_video)(AVCodecContext *avctx, AVPicture *picture,
                              int *got_picture_ptr,
                              const uint8_t *buf, int buf_size);
 void (*avcodec_flush_buffers)(AVCodecContext *avctx);
 int  (*avcodec_close)(AVCodecContext *avctx);
 void (*set_ff_idct)(void*);
 int *quant_store; 
 AVCodecContext *avctx;
 int idctOld;
public:
 TmovieSourceLibavcodec(void);
 virtual ~TmovieSourceLibavcodec() {done();};
 virtual bool init(int codecId,int AVIdx,int AVIdy);
 virtual void done(void);
 virtual int  getFrame(const TglobalSettings *global,const TpresetSettings *cfg,const unsigned char *src,unsigned int srcLen, AVPicture *avpict,int &got_picture);
 static  void getVersion(char **vers);
 virtual int* getQuant(void) {return quant_store;};
};

#endif
