#ifndef _TIMGFILTERPOSTPROC_H_
#define _TIMGFILTERPOSTPROC_H_

#include "TimgFilter.h"

class TcpuUsage;
class TmovieSource;
class Tpostproc;
class TimgFilterPostproc :public TimgFilter
{
private:
 TcpuUsage *cpu;int cpus;
public:
 TimgFilterPostproc(void);
 virtual ~TimgFilterPostproc();
 virtual void process(TtempPictures *pict,TffRect &rect,const TpresetSettings *cfg);
};

#endif
