#ifndef _TIMGFILTERPOSTPROC_H_
#define _TIMGFILTERPOSTPROC_H_

#include "TimgFilter.h"

class TcpuUsage;
class TmovieSource;
class TimgFilterPostproc :public TimgFilter
{
private:
 TcpuUsage *cpu;int cpus;
public:
 TimgFilterPostproc(void);
 virtual ~TimgFilterPostproc();
 void process(TtempPictures*,const TpresetSettings*) {};
 void process(TtempPictures *pict,TpresetSettings *cfg,bool afterResize,TmovieSource *movie);
};

#endif
