#ifndef _TFILTER_H_
#define _TFILTER_H_

#include "reg.h"
#include <vector>
#include <stddef.h>
#include "IffConstants.h"

class TimgFilter;
class TconfPage;
struct IffDecoder;
struct Tfilter
{
protected:
 struct Tparam 
  {
   Tparam(void)
    {
     val=NULL;valC=NULL;valS=NULL;
    }
   Tparam(const int *Ival)
    {
     valC=Ival;val=NULL;valS=NULL;
    }
   Tparam(int *Ival,int Imin=0,int Imax=0)
    { 
     val=Ival;valC=NULL;valS=NULL;
     min=Imin;
     max=Imax;
    } 
   Tparam(char *IvalS)
    {
     valS=IvalS;val=NULL;valC=NULL;
    } 
   int *val;const int *valC;char *valS;
   int min,max;
  };
private:
 static char *strcat3(char *buf,const char *s1,const char *s2)
  {
   strcpy(buf,s1);strcat(buf,s2);
   return buf;
  }
 void reg_op0(TregOp &t)
  {
   char pomS[256];
   t._REG_OP_N(strcat3(pomS,"is",shortName()),is,0);
   t._REG_OP_N(strcat3(pomS,"order",shortName()),order,orderDef());
   t._REG_OP_N(strcat3(pomS,"full",shortName()),full,0);
  }
 Tparam getTparam0(unsigned int id);
protected:
 virtual const int orderDef(void)=0;
 virtual const char *shortName(void)=0;
 virtual void reg_op_settings(TregOp &t)=0;
 virtual Tparam getTparam(unsigned int id)=0;
public:
 virtual int getID(void)=0;
 virtual const char *getName(void)=0;
 virtual Tfilter* duplicate(void)=0;
 int is;
 int order;
 int full;
 virtual TimgFilter *getImgFilter(void)=0;
 virtual TconfPage *getConfPage(void) {return NULL;}
 virtual TconfPage *getConfSubPage(void) {return NULL;}
 void reg_op(TregOp &t)
  {
   reg_op0(t);
   reg_op_settings(t);
  }
 bool getParam(unsigned int id,int &val);
 bool setParam(unsigned int id,int val);
 bool getParam(unsigned int id,char *val);
 bool setParam(unsigned int id,char* val);
  
 typedef std::vector<Tfilter*(*)(void)> TcreateFcVector;
 //typedef std::vector<TimgFilter*> TimgFilterVector;
 static void init(TcreateFcVector *cfcs,IffDecoder *deci),done(void);
};

#endif
