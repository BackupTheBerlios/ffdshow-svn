#ifndef _TMOVIESOURCE_H_
#define _TMOVIESOURCE_H_

struct TpresetSettings;
struct TglobalSettings;
struct AVPicture;
class TmovieSource
{
public:
 TmovieSource(void):quantDx(128),quantDy(96) {};
 virtual ~TmovieSource() {};
 static TmovieSource* initSource(int codecId,unsigned int AVIdx,unsigned int AVIdy);
 unsigned int dx,dy;
 virtual bool init(int codecId,unsigned int AVIdx,unsigned int AVIdy)=0;
 virtual void done(void)=0;
 virtual int  getFrame(const TglobalSettings *global,const TpresetSettings *preset,const unsigned char *src,unsigned int srcLen,AVPicture *avpict,int &got_picture)=0;
 unsigned int quantDx,quantDy;
 int quant[128*96];
 struct TmotionVectors
  {
   TmotionVectors(void):dx(0),dy(0),vectors(NULL) {};
   unsigned int dx,dy;
   char *vectors;
  };                         
 virtual TmotionVectors getMV(void) const  {TmotionVectors v;return v;};  
};

#endif
