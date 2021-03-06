#ifndef _TMOVIESOURCE_H_
#define _TMOVIESOURCE_H_

struct TpresetSettings;
struct TglobalSettings;
struct AVPicture;
class TmovieSource
{
public:
 TmovieSource(void):quant(NULL) {};
 virtual ~TmovieSource();
 static TmovieSource* initSource(int codecId,int AVIdx,int AVIdy);
 int dx,dy;
 virtual bool init(int codecId,int AVIdx,int AVIdy)=0;
 virtual void done(void)=0;
 virtual int  getFrame(const TglobalSettings *global,const TpresetSettings *preset,const unsigned char *src,unsigned int srcLen,AVPicture *avpict,int &got_picture)=0;
 int quantDx,quantDy;
 int *quant;
 void initQuant(void);
 struct TmotionVectors
  {
   TmotionVectors(void):dx(0),dy(0),vectors(NULL) {};
   int dx,dy;
   char *vectors;
  };
 virtual TmotionVectors getMV(void) {TmotionVectors v;return v;};  
};

#endif
