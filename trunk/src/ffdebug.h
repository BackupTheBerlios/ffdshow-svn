#ifndef _FFDEBUG_H_
#define _FFDEBUG_H_

#include <stdio.h>
#include <string.h>

#if 0
static inline void DEBUGS(const char *s)
{
 char pomS[1024];strcpy(pomS,s);strcat(pomS,"\n");
 OutputDebugString(pomS);
}
static inline void DEBUGS1(const char *s,int a)
{
 char pomS[1024];sprintf(pomS,"%s %i\n",s,a);
 OutputDebugString(pomS);
}
static inline void DEBUGS2(const char *s,int a,int b)
{
 char pomS[1024];sprintf(pomS,"%s %i, %i\n",s,a,b);
 OutputDebugString(pomS);
}
static inline void DEBUGS3(const char *s,int a,int b,int c)
{
 char pomS[1024];sprintf(pomS,"%s %i, %i, %i\n",s,a,b,c);
 OutputDebugString(pomS);
}
#else
#define DEBUGS(X)
#define DEBUGS1(X,A)
#define DEBUGS2(X,A,B)
#define DEBUGS3(X,A,B,C)
#endif

#endif
