#ifndef _TFONT_H_
#define _TFONT_H_

#include <vector>
#include <windows.h>

class Tchar
{
public:
 Tchar(HDC hdc,const char *s,int shadow,int colorY,int colorU,int colorV);
 ~Tchar();
 int dxY ,dyY ;unsigned char *bmpY ,*mskY ;
 int dxUV,dyUV;unsigned char *bmpU, *bmpV,*mskUV;
 int charDx;
};

struct subtitle;
class Tfont
{
private:
 HFONT hf;HDC hdc;
 std::vector<Tchar*> c;
 subtitle *oldsub;
 int shadow;
 int colorY,colorU,colorV;
public:
 Tfont(void);
 ~Tfont();
 void init(const char *fontName,int charset,int size,int weight,int spacing,int shadow,int color);
 void print(unsigned char *dstY,unsigned char *dstU,unsigned char *dstV,int dx,int stride,int dy,subtitle *sub,int posX,int posY);
 void done(void);
};

#endif
