#ifndef _TCPUUSAGE_H_
#define _TCPUUSAGE_H_

#include <windows.h>

class TcpuUsage
{
public:
 TcpuUsage(void)
  {
   usage_initialization();
  }
 ~TcpuUsage()
  {
   usage_finalization();
  }
 void CollectCPUData(void);  // Call CollectCPUData to refresh information about CPU usage
 int GetCPUCount(void);      // Call it to obtain the number of CPU's in the system
 int GetCPUUsage(int Index); // Call it to obtain the % of usage for given CPU
 void ReleaseCPUData(void);  // For Win9x only: call it to stop CPU usage monitoring and free system resources
private:
 void usage_initialization(void),usage_finalization(void);
 /*
 implementation

 {$ifndef ver110}

     {$ifndef ver90}
     {$ifndef ver100}
     {$define UseInt64}
     {$endif}
     {$endif}


     {$ifdef UseInt64}
     type TInt64 = Int64;
     {$else}
     type TInt64 = Comp;
     {$endif}

 {$else}

     type TInt64 = TLargeInteger;

 {$endif}
 */

 typedef __int64 TInt64;
 //type PInt64 = ^TInt64;
 typedef TInt64 *PInt64;

 //type
 typedef PERF_DATA_BLOCK TPERF_DATA_BLOCK;
 typedef PERF_OBJECT_TYPE TPERF_OBJECT_TYPE;
 typedef PERF_COUNTER_DEFINITION TPERF_COUNTER_DEFINITION;
 typedef PERF_COUNTER_BLOCK TPERF_COUNTER_BLOCK;
 typedef PERF_INSTANCE_DEFINITION TPERF_INSTANCE_DEFINITION;

 typedef TInt64 TInt64F;// = TInt64;
 /*
 {$else}
 type
     TInt64F = Extended;
 {$endif}
 */
 //{$ifdef ver110}
 //function FInt64(Value: TInt64): TInt64F;
 //function Int64D(Value: DWORD): TInt64;
 //{$else}
 //type
 #define FInt64 TInt64F
 #define Int64D TInt64
 //{$endif}
 /*
 {$ifdef ver110}
 function FInt64(Value: TInt64): TInt64F;
 var V: TInt64;
 begin
     if (Value.HighPart and $80000000) = 0 then // positive value
     begin
         result:=Value.HighPart;
         result:=result*$10000*$10000;
         result:=result+Value.LowPart;
     end else
     begin
         V.HighPart:=Value.HighPart xor $FFFFFFFF;
         V.LowPart:=Value.LowPart xor $FFFFFFFF;
         result:= -1 - FInt64(V);
     end;
 end;

 function Int64D(Value: DWORD): TInt64;
 begin
     result.LowPart:=Value;
     result.HighPart := 0; // positive only
 end;
 {$endif}
 */
 //------------------------------------------------------------------------------

 //const
 #define  Processor_IDX_Str "238"
 #define  Processor_IDX  238
 #define  CPUUsageIDX 6

 //type
 typedef TInt64F AInt64F[0xffff+1];//    AInt64F = array[0..$FFFF] of TInt64F;
 typedef TInt64F *PAInt64F;//PAInt64F = ^AInt64F;

 //var
 PPERF_DATA_BLOCK    _PerfData;
 int    _BufferSize;
 PPERF_OBJECT_TYPE   _POT;
 PPERF_COUNTER_DEFINITION _PCD;
 int    _ProcessorsCount;
 PAInt64F    _Counters;
 PAInt64F    _PrevCounters;
 TInt64F    _SysTime;
 TInt64F    _PrevSysTime;
 bool    _IsWinNT;

 bool       _W9xCollecting;
 DWORD    _W9xCpuUsage;
 HKEY    _W9xCpuKey;
 OSVERSIONINFO VI;
};

#endif
