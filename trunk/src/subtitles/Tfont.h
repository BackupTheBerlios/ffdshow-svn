#ifndef _TFONT_H_
#define _TFONT_H_

#include <vector>
#include <windows.h>

class Tchar
{
public:
 Tchar(HDC hdc,const char *s,int matrix[5][5],int colorY,int colorU,int colorV);
 static unsigned int getWidth(HDC hdc,const char *s);
 ~Tchar();
 unsigned int dxY ,dyY ;unsigned char *bmpY ,*mskY ;
 unsigned int dxUV,dyUV;unsigned char *bmpU, *bmpV,*mskUV;
 unsigned int charDx;
};

struct subtitle;
struct TpresetSettings;
class Tfont
{
private:
 HFONT hf;HDC hdc;
 std::vector<Tchar*> c;
 subtitle *oldsub;
 int shadowStrength,shadowRadius;
 int colorY,colorU,colorV;
 int matrix[5][5];
public:
 Tfont(void);
 ~Tfont();
 void init(const TpresetSettings *cfg);
 void print(unsigned char *dstY,unsigned char *dstU,unsigned char *dstV,unsigned int dx,unsigned int stride,unsigned int dy,subtitle *sub,int posX,int posY);
 void done(void);
};

#endif
