#include "common.h"
#include "avcodec.h"
#include "dsputil.h"

static char av_datetime[]=__DATE__" , "__TIME__;
void getVersion(char **version,char **build,char **datetime)
{
 if (version) *version=LIBAVCODEC_VERSION;
 if (build) *build=LIBAVCODEC_BUILD_STR;
 if (datetime) *datetime=av_datetime;
}

#ifdef FF_POSTPROCESS

int quant_store[MBR+1][MBC+1]; // [Review]

int* get_quant_store(void)
{
 return &quant_store[0][0];//[MBR+1][MBC+1]; // [Review]
};

#endif

//tabulky pre povodne hodnoty

static UINT8 zigzag_direct0[64];
static UINT8 ff_alternate_horizontal_scan0[64];
static UINT8 ff_alternate_vertical_scan0[64];
static INT16 default_intra_matrix0[64];
static INT16 default_non_intra_matrix0[64];
static INT16 ff_mpeg4_default_intra_matrix0[64];
static INT16 ff_mpeg4_default_non_intra_matrix0[64];

extern INT16 default_intra_matrix[64];
extern INT16 default_non_intra_matrix[64];
extern INT16 ff_mpeg4_default_intra_matrix[64];
extern INT16 ff_mpeg4_default_non_intra_matrix[64];

static int inited=0;
void libavcodec_init(void)
{
 if (!inited)
  {
   inited=1;
   //skopiruje do tabuliek pre povodne hodnoty tie, ktore boli na zaciatku
   memcpy(ff_alternate_horizontal_scan0,ff_alternate_horizontal_scan,64);
   memcpy(ff_alternate_vertical_scan0,ff_alternate_vertical_scan,64);
   memcpy(zigzag_direct0,zigzag_direct,64);
   memcpy(default_intra_matrix0,default_intra_matrix,128);
   memcpy(default_non_intra_matrix0,default_non_intra_matrix,128);
   memcpy(ff_mpeg4_default_intra_matrix0,ff_mpeg4_default_intra_matrix,128);
   memcpy(ff_mpeg4_default_non_intra_matrix0,ff_mpeg4_default_non_intra_matrix,128);
   avcodec_init();
   avcodec_register_all();
  };
}

//1 = simple16383
//2 = normal
//3 = simple16384
//other = address of idct routine
void set_ff_idct(void *idct)
{
 memcpy(ff_alternate_horizontal_scan,ff_alternate_horizontal_scan0,64);
 memcpy(ff_alternate_vertical_scan,ff_alternate_vertical_scan0,64);
 memcpy(zigzag_direct,zigzag_direct0,64);
 memcpy(default_intra_matrix,default_intra_matrix0,128);
 memcpy(default_non_intra_matrix,default_non_intra_matrix0,128);
 memcpy(ff_mpeg4_default_intra_matrix,ff_mpeg4_default_intra_matrix0,128);
 memcpy(ff_mpeg4_default_non_intra_matrix,ff_mpeg4_default_non_intra_matrix0,128);
 if (idct==1)      dsputil_init(1,3); //simple16383
 else if (idct==2) dsputil_init(1,0);
 else if (idct==3) dsputil_init(1,4); //simple16384 
 else
  {
   dsputil_init(0,0);
   ff_idct=idct;
  };
}

