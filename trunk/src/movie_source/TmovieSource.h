#ifndef _TMOVIESOURCE_H_
#define _TMOVIESOURCE_H_

#include "ffmpeg\libavcodec\avcodec.h"

struct TpresetSettings;
struct TglobalSettings;
class TmovieSource
{
public:
 virtual ~TmovieSource() {};
 static TmovieSource* initSource(int codecId,int AVIdx,int AVIdy);
 virtual bool init(int codecId,int AVIdx,int AVIdy)=0;
 virtual void done(void)=0;
 virtual int  getFrame(const TglobalSettings *global,const TpresetSettings *preset,const unsigned char *src,unsigned int srcLen, AVPicture *avpict,int &got_picture)=0;
 virtual int* getQuant(void) {return 0;};
};

#endif
