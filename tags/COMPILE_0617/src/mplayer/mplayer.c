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
extern int sws_flags;
extern float sws_lum_gblur;
extern float sws_chr_gblur;
extern float sws_lum_sharpen;
extern float sws_chr_sharpen;

void set_sws_params(int Isws_flags,int Isws_lum_gblur,int Isws_chr_gblur,int Isws_lum_sharpen,int Isws_chr_sharpen)
{
 sws_flags=Isws_flags;
 sws_lum_gblur  =Isws_lum_gblur  /100.0;
 sws_chr_gblur  =Isws_chr_gblur  /100.0;
 sws_lum_sharpen=Isws_lum_sharpen/100.0;
 sws_chr_sharpen=Isws_chr_sharpen/100.0;
};