unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  ExtCtrls, ComCtrls,math, StdCtrls;

type
  TForm1 = class(TForm)
    imgExp: TImage;
    tbr: TTrackBar;
    lblGamma: TLabel;
    lblA: TLabel;
    lblB: TLabel;
    lblC: TLabel;
    procedure FormCreate(Sender: TObject);
    procedure tbrChange(Sender: TObject);
  private
    x,exp,fit:array[0..255] of extended;
    procedure calc(g: Real);
    procedure draw;
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

implementation

{$R *.DFM}

uses matrices,models,curvefit;

procedure TForm1.calc(g:Real);
var i:Integer;
    a,b,c:real;
    err,err0:Real;
    a1,a2:real;
    b1,b2:Real;
    ra,rb:Real;
    s0,s1,t0,s2,s3,s4,s5,s6,t1,t2,t3:extended;
    //x,y:extended;
    xx,yy:Extended;
    a11,a12,a13,a21,a22,a23:extended;
    c0,c1,c2,c3,c4:extended;
    coeffs:array[0..5] of Extended;
    correl:Extended;
    npoints,ncoeffs:Integer;
begin
 lblGamma.Caption:=FloatToStr(g);
 g:=1/g;
 for i:=0 to 255 do begin
   x[i]:=i/255.0;
   exp[i]:=Power(x[i],g);
  end;
// PolyFit(x,exp,coeffs,correl,255,4);
 c0:=coeffs[0];c1:=coeffs[1];c2:=coeffs[2];c3:=coeffs[3];

 s0:=0;s1:=0;t0:=0;s2:=0;s3:=0;s4:=0;s5:=0;s6:=0;t1:=0;t2:=0;t3:=0;
 for i:=0 to 255 do begin
   s0:=s0+i; //i
   xx:=i/255.0;
   s1:=s1+xx; //x
   yy:=exp[i];
   t0:=t0+yy; //y
   s2:=s2+xx*xx;
   s3:=s3+xx*xx*xx;
   s4:=s4+xx*xx*xx*xx;
   s5:=s5+xx*xx*xx*xx*xx;
   s6:=s6+xx*xx*xx*xx*xx*xx;
   t1:=t1+yy*xx;
   t2:=t2+yy*xx*xx;
   t3:=t3+yy*xx*xx*xx;
  end;
 a11:=s1-s3;a12:=s2-s3;a13:=s3-t0;
 a21:=s2-s4;a22:=s3-s4;a23:=s4-t1;
 c2:=(a11*a23-a13*a21)/(a12*a21-a11*a22);
 c1:=-(a22*c2+a23)/a21;
 c3:=1-(c1+c2);
 c0:=0;

// c1:=g;c2:=1-c1;
 for i:=0 to 255 do begin
   xx:=i/255.0;
//   fit[i]:=255*(c3*xx*xx*xx+c2*xx*xx+c1*xx+c0);
//   fit[i]:=255*(coeffs[3]*xx*xx*xx+coeffs[2]*xx*xx+coeffs[1]*xx+coeffs[0]);
   xx:=1-xx;
//   fit[i]:=255*(1-(coeffs[3]*xx*xx*xx+coeffs[2]*xx*xx+coeffs[1]*xx+coeffs[0]));
   fit[i]:=255*(1-(c3*xx*xx*xx+c2*xx*xx+c1*xx+c0));
//   fit[i]:=255*(c1*xx+c2*xx*xx);
   exp[i]:=255*exp[i];
  end;
{
 b:=0;
 b1:=-100;b2:=100;
 a1:=-100;a2:=100;
 err0:=0;
 ra:=10;rb:=10;
 while (1=1) do begin
   err:=0;
   a:=(a1+a2)/2;
   b:=1-a;
   for i:=0 to 255 do begin
     fit[i]:=255*(a*Power(i/255.0,2)+b*power(i/255.0,1));//+c*(i/255.0));
     err:=err+(fit[i]-exp[i]);
    end;
   if Abs(abs(err)-abs(err0))<1  then Break;
   if err>0 then begin
     a1:=a;
     b1:=b;
    end
   else begin
     a2:=a;
     b2:=b;
    end;
  end;
}
 lblA.Caption:='c3='+FloatToStr(c3);
 lblB.Caption:='c2='+FloatToStr(c2);
 lblC.Caption:='c1='+FloatToStr(c1);
 draw;
end;

procedure TForm1.draw;
var i:integer;
    y:Integer;
begin
 imgExp.Canvas.FillRect(imgExp.Canvas.ClipRect);
 for i:=0 to 255 do begin
   imgExp.Canvas.Pixels[i,255-Trunc(exp[i])]:=clBlack;
   imgExp.Canvas.Pixels[i,255-trunc(fit[i])]:=clBlue;
  end;
end;

procedure TForm1.FormCreate(Sender: TObject);
begin
 calc(1);
end;

procedure TForm1.tbrChange(Sender: TObject);
begin
 calc(tbr.Position/10.0);
end;

end.
