#ifndef _TTEMPPICTURE_H_
#define _TTEMPPICTURE_H_

#include "xvid\utils\mem_align.h"
#include <string.h>
#include <stddef.h>

struct Trect;
/*
struct TffPict
{
 struct TtempPicture 
  {
   private:
    unsigned char *temp1,*temp2;
    unsigned char *tempCur;
    unsigned char *av;
    unsigned char c;
   public: 
    unsigned int size;
    TtempPicture(unsigned int Isize,unsigned char Ic):size(Isize),c(Ic)
     {
      temp1=temp2=NULL;
      tempCur=NULL;
     }
    ~TtempPicture()
     {
      if (temp1) xvid_free(temp1);
      if (temp2) xvid_free(temp2);
     }
    unsigned char *getTempCur(void)
     {
      return !tempCur?av:tempCur;
     }
    unsigned char *getTempNext(void)
     {
      if (!tempCur)
       {
        if (!temp1)
         {
          temp1=(unsigned char*)xvid_malloc(size,MCACHE_LINE);
          memset(temp1,c,size);
         }
        return tempCur=temp1;
       }
      if (!temp2)
       {
        temp2=(unsigned char*)xvid_malloc(size,MCACHE_LINE); 
        memset(temp2,c,size);
        return tempCur=temp2;
       }
      return tempCur=(tempCur==temp1)?temp2:temp1;
     }
    void reset(unsigned char *Iav) 
     {
      tempCur=NULL;
      av=Iav;
     }
    void clear(void)
     {
      if (temp1) memset(temp1,c,size);
      if (temp2) memset(temp2,c,size);
     } 
    unsigned char *getCurNext(unsigned int stride=0,const Trect *r=NULL,int div=0);
  } *y,*u,*v;
public:
 TffPict(unsigned int size)
  {
   y=new TtempPicture(size,0);
   u=new TtempPicture(size/4,128);
   v=new TtempPicture(size/4,128);
  }
 ~TffPict()
  {
   delete y;delete u;delete v;
  }
 void reset(unsigned char *Iy,unsigned char *Iu,unsigned char *Iv)
  {
   y->reset(Iy);
   u->reset(Iu);
   v->reset(Iv);
  }
 void clear(void)
  {
   y->clear();u->clear();v->clear();
  }
 const unsigned char *getCurY(void)
  {
   return y->getTempCur();
  }
 const unsigned char *getCurU(void)
  {
   return u->getTempCur();
  }
 const unsigned char *getCurV(void)
  {
   return v->getTempCur();
  }
 unsigned char *getNextY(void)
  {
   return y->getTempNext();
  }
 unsigned char *getNextU(void)
  {
   return u->getTempNext();
  }
 unsigned char *getNextV(void)
  {
   return v->getTempNext();
  }
 unsigned char *getCurNextY(int stride=0,const Trect *r=NULL)
  {
   return y->getCurNext(stride,r,1);
  } 
 unsigned char *getCurNextU(int stride=0,const Trect *r=NULL)
  {
   return u->getCurNext(stride,r,2);
  } 
 unsigned char *getCurNextV(int stride=0,const Trect *r=NULL)
  {
   return v->getCurNext(stride,r,2);
  } 
};
*/
#include "TffRect.h"
struct TffPict2
{
public:
 TffPict2(unsigned char *Iy,unsigned char *Iu,unsigned char *Iv,int stride,const Trect &r,bool ro):y(Iy),u(Iu),v(Iv)
  {
   roY=roU=roV=ro;
   rect.stride=stride;
   rect.full=rect.clip=r;
   rect.calcDiff();
  }
 TffRect rect;
 unsigned char *y,*u,*v;
 bool roY,roU,roV;
};

#endif
