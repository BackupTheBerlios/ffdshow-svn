#ifndef _TTEMPPICTURE_H_
#define _TTEMPPICTURE_H_

#include "..\xvid\utils\mem_align.h"

struct TtempPicture 
{
private:
 unsigned char *temp1,*temp2;
 unsigned char *tempCur;
 unsigned char *av;
 unsigned char c;
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
};

#endif