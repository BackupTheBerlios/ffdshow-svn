#ifndef _TTEMPPICTURE_H_
#define _TTEMPPICTURE_H_

#include "xvid\utils\mem_align.h"
#include <string.h>
#include <stddef.h>

struct TtempPictures
{
 struct TtempPicture 
  {
   private:
    unsigned char *temp1,*temp2;
    unsigned char *tempCur;
    unsigned char *av;
    int c;
   public: 
    int size;
    TtempPicture(int Isize,int Ic):size(Isize),c(Ic)
     {
      temp1=temp2=NULL;
      tempCur=NULL;
     };
    ~TtempPicture()
     {
      if (temp1) xvid_free(temp1);
      if (temp2) xvid_free(temp2);
     };
    unsigned char *getTempCur(void)
     {
      return !tempCur?av:tempCur;
     };
    unsigned char *getTempNext(void)
     {
      if (!tempCur)
       {
        if (!temp1)
         {
          temp1=(unsigned char*)xvid_malloc(size,MCACHE_LINE);
          memset(temp1,c,size);
         }; 
        return tempCur=temp1;
       }; 
      if (!temp2)
       {
        temp2=(unsigned char*)xvid_malloc(size,MCACHE_LINE); 
        memset(temp2,c,size);
        return tempCur=temp2;
       }; 
      return tempCur=(tempCur==temp1)?temp2:temp1;
     };
    void reset(unsigned char *Iav) 
     {
      tempCur=NULL;
      av=Iav;
     }; 
    void clear(void)
     {
      if (temp1) memset(temp1,c,size);
      if (temp2) memset(temp2,c,size);
     } 
  } *y,*u,*v;
 int diffY,diffUV;
public:
 TtempPictures(int stride,int dy,int IdiffX,int IdiffY)
  {
   dy+=16;
   y=new TtempPicture( stride   *dy+16,0);
   u=new TtempPicture((stride/2)*dy/2 ,128);
   v=new TtempPicture((stride/2)*dy/2 ,128);
   diffY = IdiffY   * stride   + IdiffX   ;
   diffUV=(IdiffY/2)*(stride/2)+(IdiffX/2);
  }
 ~TtempPictures()
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
   return y->getTempCur()+diffY;
  }
 const unsigned char *getCurU(void)
  {
   return u->getTempCur()+diffUV;
  }
 const unsigned char *getCurV(void)
  {
   return v->getTempCur()+diffUV;
  }
 unsigned char *getNextY(void)
  {
   return y->getTempNext()+diffY;
  }
 unsigned char *getNextU(void)
  {
   return u->getTempNext()+diffUV;
  }
 unsigned char *getNextV(void)
  {
   return v->getTempNext()+diffUV;
  }
};

#endif
