#ifndef _XVID_H_
#define _XVID_H_

#ifdef __cplusplus
extern "C" {
#endif 

// ==========================================
//      global
// ==========================================

// API Version: 2.1
#define API_VERSION ((2 << 16) | (1))

// cpu features
#define XVID_CPU_MMX            0x00000001
#define XVID_CPU_MMXEXT         0x00000002
#define XVID_CPU_SSE            0x00000004
#define XVID_CPU_SSE2           0x00000008
#define XVID_CPU_3DNOW          0x00000010
#define XVID_CPU_3DNOWEXT       0x00000020
#define XVID_CPU_TSC            0x00000040
#define XVID_CPU_FORCE          0x80000000

// colorspaces
#define XVID_CSP_RGB24  0
#define XVID_CSP_YV12   1
#define XVID_CSP_YUY2   2
#define XVID_CSP_UYVY   3
#define XVID_CSP_I420   4
#define XVID_CSP_RGB555 10
#define XVID_CSP_RGB565 11
#define XVID_CSP_USER   12
#define XVID_CSP_YVYU   1002
#define XVID_CSP_RGB32  1000
#define XVID_CSP_NULL   9999

#define XVID_CSP_VFLIP  0x80000000      // flip mask

// error
#define XVID_ERR_FAIL          -1
#define XVID_ERR_OK             0
#define XVID_ERR_MEMORY         1
#define XVID_ERR_FORMAT         2

typedef struct 
{
        int cpu_flags;
        int api_version;
        int core_build;
} XVID_INIT_PARAM;

int xvid_init(void *handle, int opt, void *param1, void *param2);

// ==========================================
//      decoder
// ==========================================

#define XVID_QUICK_DECODE               0x00000010 /* increases decoding speed but reduces quality */

typedef struct 
{
        int width;
        int height;
        void *handle;
} XVID_DEC_PARAM;


typedef struct
{
        int general;
        void * bitstream;
        int length;

        void * image;
        int stride;
        int colorspace;
} XVID_DEC_FRAME;


// decoder options
#define XVID_DEC_DECODE         0
#define XVID_DEC_CREATE         1
#define XVID_DEC_DESTROY        2

int xvid_decore(void * handle,
                int opt,
                void * param1,
                void * param2);

#ifdef __cplusplus
}
#endif 

#endif /* _XVID_H_ */
