#ifndef _TFFRECT_H_
#define _TFFRECT_H_

struct TffRect
{
 struct Trect
  {
   Trect(void) {x=y=dx=dy=0;};
   Trect(unsigned int Ix,unsigned int Iy,unsigned int Idx,unsigned int Idy):x(Ix),y(Iy),dx(Idx),dy(Idy) {};
   unsigned int x,y,dx,dy;
   unsigned int diffY,diffUV;
   void calcDiff(unsigned int stride)
    {
     diffY = x   + y   * stride   ;
     diffUV=(x/2)+(y/2)*(stride/2);
    }
  } full,clip;
 TffRect(void):stride(0) {};
 TffRect(unsigned int Istride,unsigned int x,unsigned int y,unsigned int dx,unsigned int dy):stride(Istride)
  {
   full=clip=Trect(x,y,dx,dy);
  };
 unsigned int stride; 
};  
inline bool operator !=(const TffRect::Trect &r1,const TffRect::Trect &r2)
{
 return (r1.x!=r2.x) || (r1.y!=r2.y) || (r1.dx!=r2.dx) || (r1.dy!=r2.dy);
}
inline bool operator !=(const TffRect &r1,const TffRect &r2)
{
 return (r1.stride!=r2.stride) || (r1.full!=r2.full) || (r1.clip!=r2.clip); 
}

#endif
