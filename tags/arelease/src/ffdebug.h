#ifndef _FFDEBUG_H_
#define _FFDEBUG_H_

#if 0
static inline void DEBUGS(const char *s)
{
 char pomS[1024];strcpy(pomS,s);strcat(pomS,"\n");
 OutputDebugString(pomS);
};
#else
#define DEBUGS(X) 
#endif

#endif