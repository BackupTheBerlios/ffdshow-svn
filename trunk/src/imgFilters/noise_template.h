 if (noiseCount&1) 
  {
   short *noiseMaskPtr=noiseMask;
   for (const unsigned char *srcEnd=src+stride*dy;src<srcEnd;src+=stride,dst+=stride)
    {
     const unsigned char *srcLnEnd=src+dx;
     __asm
      {
       mov esi,[src]
       mov edi,[dst]
       mov ecx,[noiseMaskPtr]
      lineLoop1:
       pxor mm0,mm0
       punpcklbw mm0,[esi]  
       psrlw mm0,8   
       
       paddw mm0,[ecx]
       
       pxor mm1,mm1
       packuswb mm0,mm1
       movd [edi],mm0
      
       add esi,4
       add edi,4
       add ecx,8
       cmp esi,[srcLnEnd]
       jl lineLoop1 
       mov [noiseMaskPtr],ecx
      }
    }; 
   __asm emms; 
   return;
  }; 
 short *noiseMaskPtr=noiseMask; 
 __declspec(align(8)) static __int64 noiseStrength64;
 noiseStrength64=__int64(noiseStrength) +(__int64(noiseStrength)<<16)+(__int64(noiseStrength)<<32)+(__int64(noiseStrength)<<48);
 if (!uniformNoise)
  for (const unsigned char *srcEnd=src+stride*dy;src<srcEnd;src+=stride,dst+=stride)
   {
    const unsigned char *srcLnEnd=src+dx;
    #undef NOISE_UNIFORM
    #undef  lineLoop2
    #define lineLoop2 lineLoop2u
    #include "noise_template_mmx.h"
   }
 else  
  for (const unsigned char *srcEnd=src+stride*dy;src<srcEnd;src+=stride,dst+=stride)
   {
    const unsigned char *srcLnEnd=src+dx;
    #define NOISE_UNIFORM
    #undef  lineLoop2
    #define lineLoop2 lineLoop2nu
    #include "noise_template_mmx.h"
   }
 __asm emms;
