#ifndef _TDIALOGSETTINGS_H_
#define _TDIALOGSETTINGS_H_

struct TdialogSettings
{
 int lastPage;
 int dlgRestorePos;
 int dlgPosX,dlgPosY;
 int lvWidth0;
 void load(void),save(void);
};

#endif
