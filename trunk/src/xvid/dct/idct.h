#ifndef _IDCT_H_
#define _IDCT_H_

#ifdef __cplusplus
extern "C" {
#endif 

void idct_int32_init();

typedef void (idctFunc)(short * const block);
typedef idctFunc* idctFuncPtr;	

extern idctFuncPtr xvid_idct_ptr;

idctFunc idct_int32;
idctFunc idct_mmx;
idctFunc idct_xmm;
idctFunc idct_altivec;
idctFunc idct_opendivx_mmx;
idctFunc idct_ffsimple;
idctFunc idct_famefloat;
idctFunc idct_ff;
idctFunc idct_xmmx;

void idct_ref(short *block);

char* idct_test(void);

#ifdef __cplusplus
}
#endif 

#endif /* _IDCT_H_ */
