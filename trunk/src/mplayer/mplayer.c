#include <inttypes.h>
#include "cpudetect.h"
#include "postproc/postprocess.h"
#include "postproc/swscale.h"
#include <stdio.h>

int verbose=0;
int divx_quality=0;

void (*modifyPPmodeFnc)(struct PPMode *ppmode)=NULL;
void *modifyPPmodeParam;

void init_mplayer(void)
{
 GetCpuCaps(&gCpuCaps);
}
void setModifyPPmode(void *ImodifyPPmodeFnc,void *ImodifyPPmodeParam)
{
 modifyPPmodeFnc=ImodifyPPmodeFnc;
 modifyPPmodeParam=ImodifyPPmodeParam;
}
