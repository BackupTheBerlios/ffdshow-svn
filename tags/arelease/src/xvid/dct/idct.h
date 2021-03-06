#ifndef _IDCT_H_
#define _IDCT_H_

void idct_int32_init();

typedef void (idctFunc)(short * const block);
typedef idctFunc* idctFuncPtr;	

extern idctFuncPtr idct;

idctFunc idct_int32;
idctFunc idct_mmx;
idctFunc idct_xmm;
idctFunc idct_altivec;
idctFunc idct_ref;
idctFunc idct_opendivx_mmx;
idctFunc idct_ffsimple;
idctFunc idct_famefloat;
idctFunc idct_ff;
idctFunc idct_xref;
idctFunc idct_xmmx;

char* idct_test(void);

#endif /* _IDCT_H_ */
