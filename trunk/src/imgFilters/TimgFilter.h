#ifndef _IMGFILTER_H_
#define _IMGFILTER_H_

#include "TtempPicture.h"

struct TffRect
{
 struct Trect
  {
   Trect(void) {x=y=dx=dy=0;};
   int x,y,dx,dy;
   int diffY,diffUV;
   void calcDiff(int stride)
    {
     diffY = x   + y   * stride   ;
     diffUV=(x/2)+(y/2)*(stride/2);
    }
  } full,clip;
 TffRect(void):stride(0) {};
 int stride; 
};  

bool operator !=(const TffRect::Trect &r1,const TffRect::Trect &r2)
{
 return (r1.x!=r2.x) || (r1.y!=r2.y) || (r1.dx!=r2.dx) || (r1.dy!=r2.dy);
}
bool operator !=(const TffRect &r1,const TffRect &r2)
{
 return (r1.stride!=r2.stride) || (r1.full!=r2.full) || (r1.clip!=r2.clip); ;
}

struct TpresetSettings;
struct IffDecoder;
class TimgFilter
{
private:
 TffRect oldRect;int oldFull;
protected:
 int dxY,strideY,dyY;
 int dxUV,strideUV,dyUV;
 IffDecoder *deci;
 virtual TffRect::Trect* init(TffRect *rect,int full)
  {
   if (*rect!=oldRect || oldFull!=full)
    {
     oldRect=*rect;oldFull=full;
     done();
     TffRect::Trect *r=(full==1)?&rect->full:&rect->clip;
     dxY=r->dx;dyY=r->dy;strideY=rect->stride;
     dxUV=dxY/2;dyUV=dyY/2;strideUV=strideY/2;
     r->calcDiff(rect->stride);
    }
   return (oldFull==1)?&oldRect.full:&oldRect.clip;
  };
 virtual void done(void) {};
public:
 TimgFilter(void) {deci=NULL;oldFull=-1;};
 void setDeci(IffDecoder *Ideci) {deci=Ideci;};
 virtual void process(TtempPictures *pict,TffRect &rect,const TpresetSettings *cfg)=0;
};

#endif
