/*
 * Copyright (c) 2002 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <windows.h>
#include "Tfont.h"
#include <stdlib.h>
#include "subreader.h"
#include <math.h>

using namespace std;

//========================== Tchar ===========================
Tchar::Tchar(HDC hdc,const char *s,int shadow,int colorY,int colorU,int colorV)
{
 SIZE sz;
 GetTextExtentPoint32(hdc,s,strlen(s),&sz);
 dxY=(sz.cx/4+2)*4;dyY=sz.cy+4;
 unsigned char *bmp16=(unsigned char*)calloc(dxY*3,dyY);
 HBITMAP hbmp=CreateCompatibleBitmap(hdc,dxY,dyY);
 SelectObject(hdc,hbmp);
 SetTextColor(hdc,RGB(255,255,255));
 SetBkColor(hdc,RGB(0,0,0));
 TextOut(hdc,2,2,s,strlen(s));
 BITMAPINFO bmi;
 bmi.bmiHeader.biSize=sizeof(bmi.bmiHeader);
 bmi.bmiHeader.biWidth=dxY;
 bmi.bmiHeader.biHeight=-dyY;
 bmi.bmiHeader.biPlanes=1;
 bmi.bmiHeader.biBitCount=24;
 bmi.bmiHeader.biCompression=BI_RGB;
 bmi.bmiHeader.biSizeImage=dxY*dyY;
 bmi.bmiHeader.biXPelsPerMeter=75;
 bmi.bmiHeader.biYPelsPerMeter=75;
 bmi.bmiHeader.biClrUsed=0;
 bmi.bmiHeader.biClrImportant=0;
 GetDIBits(hdc,hbmp,0,dyY,bmp16,&bmi,DIB_RGB_COLORS);
 DeleteObject(hbmp);
 int _dx,_dy;
 charDx=_dx=dxY/4+4;_dy=dyY/4+4;
 _dx=(_dx/8+1)*8;
 bmpY=(unsigned char*)calloc(_dx,_dy);
 mskY=(unsigned char*)calloc(_dx,_dy);
 int y;
 for (y=2;y<dyY-2;y+=4)
  for (int x=2;x<dxY-2;x+=4)
   {
    unsigned char *dst=bmpY+(y/4+2)*_dx+(x/4+2);
    int sum=0;
    for (int yy=y-2;yy<=y+2;yy++)
     for (int xx=x-2;xx<=x+2;xx++)
      sum+=bmp16[(yy*dxY+xx)*3];
    sum/=25;
    *dst=sum;
   }
 free(bmp16);
 dxY=_dx;dyY=_dy;
 if (shadow==100)
  memset(mskY,255,dxY*dyY);
 else
  { 
   __asm emms;
   memcpy(mskY,bmpY,dxY*dyY);
   int matrix[5][5];
   for (y=-2;y<=2;y++)
    for (int x=-2;x<=2;x++)
     {
      int d=8-(x*x+y*y);
      matrix[y+2][x+2]=1.55*shadow*pow(d/8.0,2-shadow/50.0);
     }; 
   for (y=0;y<dyY;y++)
    for (int x=0;x<dxY;x++)
     {
      int s=0,cnt=0;;
      for (int yy=-2;yy<=+2;yy++)
       {
        if (y+yy<0 || y+yy>=dyY) continue;
        for (int xx=-2;xx<=+2;xx++)
         {
          if (x+xx<0 || x+xx>=dxY) continue;
          s+=bmpY[dxY*(y+yy)+(x+xx)]*matrix[yy+2][xx+2];
          cnt++;
         };
       };   
      s/=cnt*32;  
      if (s>255) s=255;  
      mskY[dxY*y+x]=s;
     }
  }
 dxUV=dxY/2;dyUV=dyY/2;
 int _dxUV=dxUV;
 dxUV=(dxUV/8+1)*8;
 bmpU=(unsigned char*)calloc(dxUV,dyUV);bmpV=(unsigned char*)calloc(dxUV,dyUV);
 mskUV=(unsigned char*)calloc(dxUV,dyUV);
 unsigned char *bmpUptr=bmpU,*bmpVptr=bmpV;
 unsigned char *mskUVptr=mskUV;
 for (y=0;y<dyUV;y++,bmpUptr+=dxUV-_dxUV,bmpVptr+=dxUV-_dxUV,mskUVptr+=dxUV-_dxUV)
  for (int x=0;x<_dxUV;x++,bmpUptr++,bmpVptr++,mskUVptr++)
   {
    unsigned char *bmpYptr=bmpY+dxY*(y*2)+(x*2),*mskYptr=mskY+dxY*(y*2)+(x*2);
    int s;
    s =bmpYptr[0];
    s+=bmpYptr[1];
    s+=bmpYptr[dxY];
    s+=bmpYptr[dxY+1];
    *bmpUptr=(colorU*s)>>9;
    *bmpVptr=(colorV*s)>>9;
    s =mskYptr[0];
    s+=mskYptr[1];
    s+=mskYptr[dxY];
    s+=mskYptr[dxY+1];
    *mskUVptr=s/8;
   }
 for (int i=0;i<dxY*dyY;i++) bmpY[i]=(bmpY[i]*colorY)>>8;
}  
Tchar::~Tchar()
{
 free(bmpY);free(mskY);
 free(bmpU);free(bmpV);free(mskUV);
}

//========================== Tfont ===========================
Tfont::Tfont(void)
{
 oldsub=NULL;
 hf=NULL;hdc=NULL;
}
Tfont::~Tfont()
{
 done();
}
void Tfont::init(const char *fontName,int charset,int size,int weight,int spacing,int Ishadow,int color)
{
 done();
 hf=CreateFont(size*4,0,0,0,weight,0,0,0,charset,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,/*ANTIALIASED_QUALITY*/DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,fontName);
 shadow=Ishadow;
 __asm emms;
 double R=GetRValue(color)/255.0,G=GetGValue(color)/255.0,B=GetBValue(color)/255.0;
 double Y=0.299*R + 0.587*G + 0.114*B;
 double U=(B-Y)*0.565;
 double V=(R-Y)*0.713;
 colorY=Y*255;colorU=U*127;colorV=V*127;
 if (!hf) return;
 hdc=CreateCompatibleDC(NULL);
 if (!hdc) return;
 SelectObject(hdc,hf);
 SetTextCharacterExtra(hdc,spacing);
}
void Tfont::done(void)
{
 DeleteObject(hf);hf=NULL;
 DeleteDC(hdc); hdc=NULL;
 oldsub=NULL;
}
void Tfont::print(unsigned char *dstY,unsigned char *dstU,unsigned char *dstV,int dx,int stride,int dy,subtitle *sub,int posX,int posY)
{
 if (!sub) return;
 if (oldsub!=sub)
  {
   oldsub=sub;
   int i;
   for (i=0;i<c.size();i++) delete c[i];c.clear();
   if (!sub) return;
   for (i=0;i<sub->lines;i++)
    c.push_back(new Tchar(hdc,sub->text[i],shadow,colorY,colorU,colorV));
  }; 
 if (c.empty()) return;
 unsigned int i;
 int h=0;
 for (i=0;i<c.size();i++) h+=c[i]->dyY;
 int y=(posY*dy)/100-h/2;if (y<0) y=0;if (y+h>=dy) y=dy-h;
 for (i=0;i<c.size();y+=c[i]->dyY-2,i++)
  {
   int x=(posX*dx)/100-c[i]->charDx/2;if (x<0) x=0;if (x+c[i]->charDx>=dx) x=dx-c[i]->dxY;
   unsigned char *msk=c[i]->mskY,*bmp=c[i]->bmpY;
   unsigned char *dstLn=dstY+stride*y;
   int yy;
   for (yy=0;yy<c[i]->dyY;yy++,dstLn+=stride)
    {
     unsigned char *dstPix=dstLn+x;
     #if 0
     for (int xx=0;xx<c[i]->dxY;xx++,dstPix++,msk++,bmp++)
      {
       int c=*dstPix;
       c-=*msk;if (c<0) c=0;
       c+=(*bmp*colorY)>>8;if (c>255) c=255;      
       *dstPix=c;
      }
     #else
     int dxY=c[i]->dxY>>3;
     __asm
      {
       mov ecx,[dxY]
       mov edx,[bmp]         //edx - bmp
       mov esi,[msk]         //esi - msk
       mov edi,[dstPix]      //edi - dstPix
      lumFont1:
       movq  mm0,[edi]        //mm0 = c
       psubusb mm0,[esi]
       add   esi,8
       paddusb mm0,[edx]
       add   edx,8
       movq  [edi],mm0
       add   edi,8
       dec   ecx
       jnz   lumFont1
       mov   [bmp],edx
       mov   [msk],esi
      }
     #endif
    };
   unsigned char *dstLnU=dstU+(stride/2)*(y/2),*dstLnV=dstV+(stride/2)*(y/2);
   msk=c[i]->mskUV;unsigned char *bmpU=c[i]->bmpU,*bmpV=c[i]->bmpV;
   for (yy=0;yy<c[i]->dyUV;yy++,dstLnU+=stride/2,dstLnV+=stride/2)
    {
     unsigned char *dstPixU=dstLnU+x/2,*dstPixV=dstLnV+x/2;
     #if 0
     for (int xx=0;xx<c[i]->dxUV;xx++,dstPixU++,dstPixV++,msk++,bmp++)
      {
       int m=*msk,b=*bmp;
       int c;
       c=*dstPixU;
       c-=128;
       if (c<0) {c+=m;if (c>0) c=0;}
       else     {c-=m;if (c<0) c=0;};
       c+=(b*colorU)>>6;
       c+=128;
       *dstPixU=c;
       
       c=*dstPixV;
       c-=128;
       if (c<0) {c+=m;if (c>0) c=0;}
       else     {c-=m;if (c<0) c=0;};
       c+=(b*colorV)>>6;
       c+=128;
       *dstPixV=c;
      }
     #else
     int dxUV=c[i]->dxUV/8;
     __declspec(align(8)) static const __int64 m128=0x8080808080808080;
     __declspec(align(8)) static const __int64 m255=0xffffffffffffffff;
     __asm 
      {
       mov ebx,[msk]
       mov ecx,[bmpU]
       mov edx,[bmpV]
       mov esi,[dstPixU]
       mov edi,[dstPixV]
       mov eax,[dxUV]
       movq mm5,[m128]
      lumChrom1:
       movq  mm0,[esi]
       movq  mm1,[edi]
       
       psubb mm0,mm5
       psubb mm1,mm5
       
       movq    mm6,[ebx] //mm6 - mask
       pxor    mm7,mm7
       pcmpgtb mm7,mm0   //mm7 - co treba znegovat
       movq    mm3,mm0
       paddusb mm3,mm6 
       movq    mm4,mm0
       psubusb mm4,mm6
       pand    mm3,mm7
       pxor    mm7,[m255]
       pand    mm4,mm7
       por     mm3,mm4
       movq    mm0,mm3

       movq    mm6,[ebx] //mm6 - mask
       pxor    mm7,mm7
       pcmpgtb mm7,mm1   //mm7 - co treba znegovat
       movq    mm3,mm1
       paddusb mm3,mm6 
       movq    mm4,mm1
       psubusb mm4,mm6
       pand    mm3,mm7
       pxor    mm7,[m255]
       pand    mm4,mm7
       por     mm3,mm4
       movq    mm1,mm3
      
       add    ebx,8
       
       paddb mm0,[ecx]
       add   ecx,8
       paddb mm1,[edx]
       add   edx,8
       
       paddb mm0,mm5
       paddb mm1,mm5
       
       movq  [esi],mm0
       add   esi,8
       movq  [edi],mm1  
       add   edi,8
       dec   eax
       jnz  lumChrom1
       mov  [msk],ebx
       mov  [bmpU],ecx
       mov  [bmpV],edx
      }
     #endif
    } 
  } 
 __asm emms; 
}
