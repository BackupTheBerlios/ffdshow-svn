#ifndef _TMOVIESOURCEXVID_H_
#define _TMOVIESOURCEXVID_H_

#include "TmovieSource.h"

class Tdll;
class TmovieSourceXviD :public TmovieSource
{
private:
 Tdll *dll;
 int (*xvid_init  )(void *handle, int opt, void *param1, void *param2);
 int (*xvid_decore)(void * handle, int opt, void * param1, void * param2);
 void *dechandle;
 int stride;
public:
 TmovieSourceXviD(void);
 virtual ~TmovieSourceXviD() {done();}
 virtual bool init(int codecId,unsigned int AVIdx,unsigned int AVIdy);
 virtual void done(void);
 virtual TffPict2 getFrame(const TglobalSettings *global,const TpresetSettings *preset,const unsigned char *src,unsigned int srcLen,int &used_bytes,int &got_picture);
 static  bool getVersion(char **vers);
};

#endif
