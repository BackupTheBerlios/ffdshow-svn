#ifndef _REG_H_
#define _REG_H_

#define FFDSHOW_REG_PARENT "Software\\GNU"
#define FFDSHOW_REG_CHILD  "ffdshow"
#define FFDSHOW_REG_CLASS  "config"

struct TregOp
{
public:
 virtual void _REG_OP_N(const char *X,int &Y,const int Z)=0;
 virtual void _REG_OP_S(const char *X,char *Y,const char *Z)=0;
};
struct TregOpRegRead :public TregOp
{
private:
 HKEY hKey;
public:
 TregOpRegRead(HKEY hive,const char *key)
  {
   RegOpenKeyEx(hive,key,0,KEY_READ,&hKey);
  }
 ~TregOpRegRead()
  {
   if (hKey) RegCloseKey(hKey);
  }
 virtual void _REG_OP_N(const char *X,int &Y,const int Z)
  {
   DWORD size=sizeof(int);
   if (RegQueryValueEx(hKey,X,0,0,(LPBYTE)&Y,&size)!=ERROR_SUCCESS)
    Y=Z;
  }
 virtual void _REG_OP_S(const char *X,char *Y,const char *Z)
  {
   DWORD size=MAX_PATH;
   if (RegQueryValueEx(hKey,X,0,0,(unsigned char*)Y,&size)!=ERROR_SUCCESS)
    strcpy(Y,Z);  
  } 
};
struct TregOpRegWrite :public TregOp
{
private:
 HKEY hKey;
public:
 TregOpRegWrite(HKEY hive,const char *key)
  {
   DWORD dispo;
   if (RegCreateKeyEx(hive,key,0,FFDSHOW_REG_CLASS,REG_OPTION_NON_VOLATILE,KEY_WRITE,0,&hKey,&dispo)!=ERROR_SUCCESS) hKey=NULL;
  }
 ~TregOpRegWrite()
  {
   if (hKey) RegCloseKey(hKey);
  }
 virtual void _REG_OP_N(const char *X,int &Y,const int)
  {
   RegSetValueEx(hKey,X,0,REG_DWORD,(LPBYTE)&Y,sizeof(int));
  }
 virtual void _REG_OP_S(const char *X,char *Y,const char *)
  {
   RegSetValueEx(hKey,X,0,REG_SZ,(unsigned char*)Y,strlen(Y)+1);
  }
};
struct TregOpFileRead :public TregOp
{
private:
 char flnm[260],section[260];
 char pomS[256],propS[256];
public:
 TregOpFileRead(const char *Iflnm,const char *Isection)
  {
   strcpy(flnm,Iflnm);
   strcpy(section,Isection);
  }
 virtual void _REG_OP_N(const char *X,int &Y,const int Z)
  {
   GetPrivateProfileString(section,X,_itoa(Z,pomS,10),propS,255,flnm);
   Y=atoi(propS);
  }
 virtual void _REG_OP_S(const char *X,char *Y,const char *Z)
  {
   GetPrivateProfileString(section,X,Z,Y,255,flnm);
  }
};
struct TregOpFileWrite :public TregOp
{
private:
 char flnm[260],section[260];
 char pomS[256];
public:
 TregOpFileWrite(const char *Iflnm,const char *Isection)
  {
   strcpy(flnm,Iflnm);
   strcpy(section,Isection);
  }
 virtual void _REG_OP_N(const char *X,int &Y,const int)
  {
   WritePrivateProfileString(section,X,_itoa(Y,pomS,10),flnm);
  }
 virtual void _REG_OP_S(const char *X,char *Y,const char *)
  {
   WritePrivateProfileString(section,X,Y,flnm);
  }
};

#endif
