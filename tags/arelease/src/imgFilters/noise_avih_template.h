 if (!cfg->uniformNoise)
  for (unsigned char *srcEnd=src+stride*dy;src<srcEnd;src+=stride,dst+=stride)
   {
    unsigned char *srcLnEnd=src+dx;
    #undef NOISE_UNIFORM
    #undef  lineLoop2
    #define lineLoop2 lineLoop2u
    #include "noise_template_mmx.h"
   }
 else  
  for (unsigned char *srcEnd=src+stride*dy;src<srcEnd;src+=stride,dst+=stride)
   {
    unsigned char *srcLnEnd=src+dx;
    #define NOISE_UNIFORM
    #undef  lineLoop2
    #define lineLoop2 lineLoop2nu
    #include "noise_template_mmx.h"
   }
