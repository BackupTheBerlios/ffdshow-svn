#ifndef _TMOVIESOURCEUNCOMPRESSED_H_
#define _TMOVIESOURCEUNCOMPRESSED_H_

#include "TmovieSource.h"

class TmovieSourceUncompressed :public TmovieSource
{
private:
 int csp;
 unsigned char *yuvY,*yuvU,*yuvV;
 unsigned int stride;
public:
 TmovieSourceUncompressed(void);
 virtual ~TmovieSourceUncompressed() {done();};
 virtual bool init(int codecId,unsigned int AVIdx,unsigned int AVIdy);
 virtual void done(void);
 virtual int  getFrame(const TglobalSettings *global,const TpresetSettings *cfg,const unsigned char *src,unsigned int srcLen, AVPicture *avpict,int &got_picture);
 static  void getVersion(char **vers);
};

#endif
