#ifndef _TCONFIG_H_
#define _TCONFIG_H_

class Tconfig
{
public:
 void init(void);
 char pth[1024];
 static int cpu_flags;
};

extern Tconfig config;

#endif
