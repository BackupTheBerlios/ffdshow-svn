#include <windows.h>
#include "reg.h"
#include "TdialogSettings.h"

void TdialogSettings::load(void)
{
 HKEY hKey;DWORD size;
 RegOpenKeyEx(HKEY_CURRENT_USER, FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD,0,KEY_READ,&hKey);
 REG_GET_N("lastPage",lastPage,0);
 RegCloseKey(hKey); 
}

void TdialogSettings::save(void)
{
 HKEY hKey;DWORD dispo;
 if (RegCreateKeyEx(HKEY_CURRENT_USER,FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD,0,FFDSHOW_REG_CLASS,REG_OPTION_NON_VOLATILE,KEY_WRITE,0,&hKey,&dispo)==ERROR_SUCCESS)
  {
   REG_SET_N("lastPage",lastPage,0);
   RegCloseKey(hKey);
  }; 
}
