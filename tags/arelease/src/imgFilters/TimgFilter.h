#ifndef _IMGFILTER_H_
#define _IMGFILTER_H_

class Tconfig;
class TimgFilter
{
protected:
 int dxY,strideY,dyY;
 int dxUV,strideUV,dyUV;
public:
 virtual ~TimgFilter()
  {
   done();
  }
 void init(int Idx,int Istride,int Idy)
  {
   done();
   dxY=Idx;dyY=Idy;strideY=Istride;
   dxUV=Idx/2;dyUV=Idy/2;strideUV=Istride/2;
  };
 virtual void done(void) {};
 virtual void process(unsigned char *srcY,unsigned char *srcU,unsigned char *srcV,
                      unsigned char *dstY,unsigned char *dstU,unsigned char *dstV,
                      Tconfig *cfg)=0;
};

#endif
