#ifndef _TSUBTITLES_H_
#define _TSUBTITLES_H_

#include "subreader.h"

class Tfont;
class Tsubtitles
{
private:
 subtitle *subs;
 float origFps,userFps;
 int delay;
 subtitle *oldsub;
 int current_sub;
 unsigned long nosub_range_start;
 unsigned long nosub_range_end;

 subtitle *findSubtitle(int frame,subtitle *begin);
public:
 Tsubtitles(void);
 ~Tsubtitles();
 void init(const char *aviFlnm,const char *subFlnm,float Ifps);
 void done(void);
 void setFps(float Ifps),setDelay(int Idelay);
 subtitle* getSubtitle(unsigned long frame);
 char flnm[1024];
};

#endif
