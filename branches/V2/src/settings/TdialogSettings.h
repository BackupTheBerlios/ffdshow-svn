#ifndef _TDIALOGSETTINGS_H_
#define _TDIALOGSETTINGS_H_

struct TregOp;
struct TdialogSettings
{
private:
 void reg_op(TregOp &t);
public: 
 int lastPage;
 int dlgRestorePos;
 int dlgPosX,dlgPosY;
 int lvWidth0;
 int showHints;
 void load(void),save(void);
};

#endif
