#ifndef _TDLL_H_
#define _TDLL_H_

#include "Tconfig.h"
#include <stdio.h>

class Tdll
{
private:
 HMODULE hdll;
 void loadDll(const char *dllName);
public:
 bool ok;
 Tdll(const char *dllName1)
  {
   hdll=LoadLibrary(dllName1);
   if (!hdll)
    {
     char name[255],ext[255];
     _splitpath(dllName1,NULL,NULL,name,ext);
     char dllName2[1024];
     _makepath(dllName2,NULL,NULL,name,ext);
     hdll=LoadLibrary(dllName2);
     if (!hdll)
      {
       char pomS[256];
       sprintf(pomS,"%s%s",config.pth,dllName2);
       hdll=LoadLibrary(pomS);
      }
    }
   ok=(hdll!=NULL);
  };
 ~Tdll()
  {
   if (hdll)
    FreeLibrary(hdll);
  }
 void loadFunction(void **fnc,const char *name)
  {
   *fnc=GetProcAddress(hdll,name);
   ok&=(*fnc!=NULL);
  };
};

#endif
