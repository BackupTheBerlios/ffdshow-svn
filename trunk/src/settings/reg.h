#ifndef _REG_H_
#define _REG_H_

#define FFDSHOW_REG_PARENT "Software\\GNU"
#define FFDSHOW_REG_CHILD  "ffdshow"
#define FFDSHOW_REG_CLASS  "config"

#define REG_GET_N(X, Y, Z) size=sizeof(int);if(RegQueryValueEx(hKey, X, 0, 0, (LPBYTE)&Y, &size) != ERROR_SUCCESS) {Y=Z;}
#define REG_GET_N_FILE(X, Y, Z) {GetPrivateProfileString(sections,X,_itoa(Z,pomS,10),propS,255,flnm);Y=atoi(propS);}
#define REG_GET_S(X, Y, Z) size=MAX_PATH;if(RegQueryValueEx(hKey, X, 0, 0, (unsigned char*)(Y), &size) != ERROR_SUCCESS) {lstrcpy(Y, Z);}
#define REG_GET_S_FILE(X, Y, Z) {GetPrivateProfileString(sections,X,Z,propS,255,flnm);strcpy(Y,propS);}

#define REG_SET_N(X, Y, Z) RegSetValueEx(hKey, X, 0, REG_DWORD, (LPBYTE)&Y, sizeof(int))
#define REG_SET_N_FILE(X, Y, Z) WritePrivateProfileString(presetName,X,_itoa(Y,pomS,10),flnm)
#define REG_SET_S(X, Y ,Z) RegSetValueEx(hKey, X, 0, REG_SZ, (unsigned char*)(Y), lstrlen(Y)+1)
#define REG_SET_S_FILE(X, Y, Z) WritePrivateProfileString(presetName,X,Y,flnm)

#endif