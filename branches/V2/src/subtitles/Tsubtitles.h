#ifndef _TSUBTITLES_H_
#define _TSUBTITLES_H_

#include "subreader.h"

class Tfont;
class Tsubtitles
{
private:
 subtitle *subs;
 double origFps,userFps;
 int delay;
 subtitle *oldsub;
 int current_sub;
 int nosub_range_start;
 int nosub_range_end;

 subtitle *findSubtitle(int frame,subtitle *begin);
public:
 Tsubtitles(void);
 ~Tsubtitles();
 void init(const char *aviFlnm,const char *subFlnm,double Ifps);
 void done(void);
 void setFps(double Ifps),setDelay(int Idelay);
 subtitle* getSubtitle(int frame);
 char flnm[260];
};

#endif
