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
    exp:array[0..255] of extended;
    fit:array[0..255] of extended;
    procedure calc1(g: Real);
    procedure calc2(g: real);
    procedure draw;
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

implementation

{$R *.DFM}

procedure TForm1.calc1(g:Real);
var i:Integer;
    s0,s1,t0,s2,s3,s4,s5,s6,t1,t2,t3:extended;
    xx,yy:Extended;
    a11,a12,a13,a21,a22,a23:extended;
    c1,c2,c3:extended;
    c1i,c2i,c3i:integer;
    xxi,k:integer;
begin
 lblGamma.Caption:=FloatToStr(g);
 for i:=0 to 255 do begin
   exp[i]:=Power(i/255.0,g);
  end;

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

 c1:=(t0-s2)/(s1-s2);
 c2:=1-c1;c3:=0;
 for i:=0 to 255 do begin
   exp[i]:=255*Power(i/255.0,g);
   xx:=i/255.0;
   fit[i]:=round(255*(c3*xx*xx*xx+c2*xx*xx+c1*xx));
  end;

 lblA.Caption:='c3='+FloatToStr(c3);
 lblB.Caption:='c2='+FloatToStr(c2);
 lblC.Caption:='c1='+FloatToStr(c1);
 draw;
end;

procedure TForm1.calc2(g: real);
var i:integer;
    a,a1,a2,b:real;
    err0,err:real;
    xx:real;
begin
 lblGamma.Caption:=FloatToStr(g);
 for i:=0 to 255 do begin
   exp[i]:=Power(i/255.0,g);
  end;
 a1:=-100;a2:=100;
 err0:=0;
 while (1=1) do begin
   err:=0;
   a:=(a1+a2)/2;
   b:=1-a;
   for i:=0 to 255 do begin
     xx:=i/255.0;
     fit[i]:=b*xx*xx+a*xx;
     err:=err+(fit[i]-exp[i]);
    end;
   if Abs(abs(err)-abs(err0))<1e-34  then Break;
   if err<0 then begin
     a1:=a;
    end
   else begin
     a2:=a;
    end;
   err0:=err; 
  end;
 b:=1-a;
 for i:=0 to 255 do begin
   exp[i]:=255*Power(i/255.0,g);
   xx:=i/255.0;
   fit[i]:=round(255*(b *xx*xx+a *xx));
  end;
 lblB.Caption:='c2='+FloatToStr(b);
 lblC.Caption:='c1='+FloatToStr(a);
 draw;
end;

procedure TForm1.draw;
var i:integer;
begin
 imgExp.Canvas.FillRect(imgExp.Canvas.ClipRect);
 for i:=0 to 255 do begin
   imgExp.Canvas.Pixels[i,255-Trunc(exp[i])]:=clBlack;
   imgExp.Canvas.Pixels[i,255-trunc(fit[i])]:=clBlue;
  end;
end;

procedure TForm1.FormCreate(Sender: TObject);
begin
 calc1(1);
end;

procedure TForm1.tbrChange(Sender: TObject);
begin
 calc2(tbr.Position/10.0);
end;

end.
