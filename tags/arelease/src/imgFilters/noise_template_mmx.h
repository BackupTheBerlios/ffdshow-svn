__asm
 {
  mov eax,[srcLnEnd]
  mov esi,[src]
  mov edi,[dst]
  mov ecx,[noiseMaskPtr]
  #ifndef NOISE_AVIH
  movq mm6,[noiseStrength64]
  movq mm5,[noiseConst]
  movq mm4,[noisenext]
  #endif
  movq mm2,[m255]
  movq mm3,[m128]
 lineLoop2:
  #ifndef NOISE_AVIH
  PMADDWD mm4,mm5
  PADDW mm4,mm5
  movq mm1,mm4
  pand mm1,mm2
  psubsw mm1,mm3       //mm1 = nois 
  #else
  movq mm1,[ecx]          //mm1 = nois * strength
  #endif
 
  pxor mm0,mm0
  punpcklbw mm0,[esi]  
  psrlw mm0,8          //mm0 = srcLn[0]
  movq mm7,mm0         //mm7 = srcLn[0]
 
#ifdef NOISE_UNIFORM 
  movq mm0,mm1
#else
 #ifdef NOISE_CHROMA
  PSUBsW mm0,mm3       //mm9 = srcLn[0]-128
 #endif
  PMULLW mm0,mm1     
  PSRAW  mm0,8         //mm0 = (nois*srcLn[0])/256; 
#endif

  #ifndef NOISE_AVIH
  PMULLW mm0,mm6
  PSRAW  mm0,8         //mm0 = nois = (nois*noiseStrength)/256;
  #endif
 
  #ifndef NOISE_AVIH
  movq [ecx],mm0
  #endif
 
  PADDW  mm0,mm7       //mm0 = res = srcLn[0]+nois;
  pxor mm7,mm7
  packuswb mm0,mm7
  movd [edi],mm0

  add esi,4
  add edi,4
  add ecx,8
  cmp esi,eax
  jl  lineLoop2
  mov [noiseMaskPtr],ecx
  #ifndef NOISE_AVIH
  movq [noisenext],mm4
  #endif
 };
