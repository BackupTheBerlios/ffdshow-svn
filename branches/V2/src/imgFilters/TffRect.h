#ifndef _TFFRECT_H_
#define _TFFRECT_H_

struct Trect
{
 Trect(void) {x=y=dx=dy=diffY=diffUV=0;}
 Trect(unsigned int Ix,unsigned int Iy,unsigned int Idx,unsigned int Idy):x(Ix),y(Iy),dx(Idx),dy(Idy) {}
 Trect(unsigned int Ix,unsigned int Iy,unsigned int Idx,unsigned int Idy,unsigned int stride):x(Ix),y(Iy),dx(Idx),dy(Idy) {calcDiff(stride);}
 unsigned int x,y,dx,dy;
 unsigned int diffY,diffUV;
 void calcDiff(unsigned int stride)
  {
   diffY = x   + y   * stride   ;
   diffUV=(x/2)+(y/2)*(stride/2);
  }
};
  
struct TffRect
{
 Trect full,clip;
 TffRect(void):stride(0) {}
 TffRect(unsigned int Istride,unsigned int x,unsigned int y,unsigned int dx,unsigned int dy):stride(Istride)
  {
   full=clip=Trect(x,y,dx,dy);full.calcDiff(stride);clip.calcDiff(stride);
  }
 unsigned int stride; 
 void clear(unsigned char *y,unsigned char *u,unsigned char *v);
 void calcDiff(void) {full.calcDiff(stride);clip.calcDiff(stride);}
};  
inline bool operator !=(const Trect &r1,const Trect &r2)
{
 return (r1.x!=r2.x) || (r1.y!=r2.y) || (r1.dx!=r2.dx) || (r1.dy!=r2.dy);
}
inline bool operator ==(const Trect &r1,const Trect &r2)
{
 return (r1.x==r2.x) && (r1.y==r2.y) && (r1.dx==r2.dx) && (r1.dy==r2.dy);
}

inline bool operator !=(const TffRect &r1,const TffRect &r2)
{
 return (r1.stride!=r2.stride) || (r1.full!=r2.full) || (r1.clip!=r2.clip); 
}
inline bool operator ==(const TffRect &r1,const TffRect &r2)
{
 return (r1.stride==r2.stride) && (r1.full==r2.full) && (r1.clip==r2.clip); 
}

#endif
