#ifndef _TXVID_H_
#define _TXVID_H_

#include <assert.h>
#include "Tdll.h"

class Tconfig;
class Txvid
{
private:
 Tdll *xvid_dll;
public:
 Txvid()
  {
   ok=inited=false;
   xvid_dll=NULL;
  };
 bool ok,inited;
 void init(void);
 void done(void);
 int (*xvid_init)(void *handle, int opt, void *param1, void *param2);
 int (*xvid_decore)(void * handle, int opt, void * param1, void * param2);
};

#endif
