#ifndef _IMGFILTER_H_
#define _IMGFILTER_H_

#include "TffRect.h"
#include "TffPict.h"

struct TpresetSettings;
struct IffDecoder;
class TimgFilter
{
private:
 TffRect oldRect;int oldFull;
 void allocY(const TffRect *rect)
  {
   int size=((rect->stride/16+2)*16)*rect->full.dy;
   y=(unsigned char*)xvid_malloc(size  ,MCACHE_LINE);
   memset(y,0,size);
  };
 void allocU(const TffRect *rect)
  {
   int size=((rect->stride/16+2)*16)*rect->full.dy;
   u=(unsigned char*)xvid_malloc(size/4,MCACHE_LINE);
   memset(u,128,size/4);
  }
 void allocV(const TffRect *rect)
  {
   int size=((rect->stride/16+2)*16)*rect->full.dy;
   v=(unsigned char*)xvid_malloc(size/4,MCACHE_LINE);
   memset(v,128,size/4);
  }
 void freeYUV(void)
  {
   if (y) xvid_free(y);y=NULL;
   if (u) xvid_free(u);u=NULL;
   if (v) xvid_free(v);v=NULL;
  }
 void copyY(unsigned char *dst,const unsigned char *src,const Trect &r,unsigned int strideY)
  {
   for (unsigned int y=r.y*strideY;y<r.dy*strideY;y+=strideY)
    memcpy(dst+y+r.x,src+y+r.x,r.dx);
  } 
 void copyUV(unsigned char *dst,const unsigned char *src,const Trect &r,unsigned int strideUV)
  {
   for (unsigned int y=(r.y/2)*strideUV;y<(r.dy/2)*strideUV;y+=strideUV)
    memcpy(dst+y+r.x/2,src+y+r.x/2,r.dx/2);
  } 
protected:
 unsigned char *y,*u,*v;
 unsigned int dxY,strideY,dyY;
 unsigned int dxUV,strideUV,dyUV;
 IffDecoder *deci;
 virtual Trect* init(TffRect *rect,int full)
  {
   if (*rect!=oldRect || oldFull!=full)
    {
     done();freeYUV();
     Trect *r=(full==1)?&rect->full:&rect->clip;
     dxY=r->dx;dyY=r->dy;strideY=rect->stride;
     dxUV=dxY/2;dyUV=dyY/2;strideUV=strideY/2;
     r->calcDiff(rect->stride);
     oldRect=*rect;oldFull=full;
    }
   return (oldFull==1)?&rect->full:&rect->clip;
  }
 const unsigned char *getCurY(TffPict2 &pict) {return pict.y;}
 const unsigned char *getCurU(TffPict2 &pict) {return pict.u;}
 const unsigned char *getCurV(TffPict2 &pict) {return pict.v;}
 unsigned char *getNextY(TffPict2 &pict,const TffRect *rect2=NULL)
  {
   if (!y) allocY(rect2?rect2:&pict.rect);
   return pict.y=y;
  }
 unsigned char *getNextU(TffPict2 &pict,const TffRect *rect2=NULL)
  {
   if (!u) allocU(rect2?rect2:&pict.rect);
   return pict.u=u;
  }
 unsigned char *getNextV(TffPict2 &pict,const TffRect *rect2=NULL)
  {
   if (!v) allocV(rect2?rect2:&pict.rect);
   return pict.v=v;
  }
 unsigned char *getCurNextY(TffPict2 &pict) 
  {
   if (pict.ro)
    {
     if (!y) allocY(&pict.rect);
     copyY(y,pict.y,pict.rect.full,pict.rect.stride);
     return pict.y=y;
    }
   else return pict.y;
  }
 unsigned char *getCurNextU(TffPict2 &pict) 
  {
   if (pict.ro)
    {
     if (!u) allocU(&pict.rect);
     copyUV(u,pict.u,pict.rect.full,pict.rect.stride/2);
     return pict.u=u;
    }
   else return pict.u;
  }
 unsigned char *getCurNextV(TffPict2 &pict)
  {
   if (pict.ro)
    {
     if (!v) allocV(&pict.rect);
     copyUV(v,pict.v,pict.rect.full,pict.rect.stride/2);
     return pict.v=v;
    }
   else return pict.v;
  }
public:
 TimgFilter(void) {deci=NULL;oldFull=-1;y=u=v=NULL;}
 virtual ~TimgFilter() {freeYUV();}
 virtual void done(void) {}
 void setDeci(IffDecoder *Ideci) {deci=Ideci;}
 virtual void process(TffPict2 &pict,const TpresetSettings *cfg)=0;
};

#endif
