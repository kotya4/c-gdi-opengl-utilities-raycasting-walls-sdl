
#include <windows.h>
#include <iphlpapi.h>
#include <stdio.h>


int
print_GetExtendedTcpTable_error ( DWORD status ) {
  if ( status == ERROR_INSUFFICIENT_BUFFER ) {
    printf ( "GetExtendedTcpTable: ERROR_INSUFFICIENT_BUFFER\n" );
    return -1;
  }
  else if ( status == ERROR_INVALID_PARAMETER ) {
    printf ( "GetExtendedTcpTable: ERROR_INVALID_PARAMETER\n" );
    return -1;
  }
  else if ( status == ERROR_NOT_SUPPORTED ) {
    printf ( "GetExtendedTcpTable: ERROR_NOT_SUPPORTED\n" );
    return -1;
  }
  else if ( status != NO_ERROR ) {
    printf ( "GetExtendedTcpTable: unparsed error %d\n", status );
    return -1;
  }
  return 0;
}


int
print_GetPerTcpConnectionEStats_error ( ULONG status ) {
  if ( status == ERROR_INSUFFICIENT_BUFFER ) {
    printf ( "GetPerTcp[6]ConnectionEStats: ERROR_INSUFFICIENT_BUFFER\n" );
    return -1;
  }
  else if ( status == ERROR_INVALID_PARAMETER ) {
    printf ( "GetPerTcp[6]ConnectionEStats: ERROR_INVALID_PARAMETER\n" );
    return -1;
  }
  else if ( status == ERROR_INVALID_USER_BUFFER ) {
    printf ( "GetPerTcp[6]ConnectionEStats: ERROR_INVALID_USER_BUFFER\n" );
    return -1;
  }
  else if ( status == ERROR_NOT_FOUND ) {
    printf ( "GetPerTcp[6]ConnectionEStats: ERROR_NOT_FOUND\n" );
    return -1;
  }
  else if ( status == ERROR_NOT_SUPPORTED ) {
    printf ( "GetPerTcp[6]ConnectionEStats: ERROR_NOT_SUPPORTED\n" );
    return -1;
  }
  else if ( status == ERROR_NOT_SUPPORTED ) {
    printf ( "GetPerTcp[6]ConnectionEStats: ERROR_NOT_SUPPORTED\n" );
    return -1;
  }
}


int
main () {
  
  // GetExtendedTcpTable
  
  DWORD dwstatus;
  
  // pid
  printf ( "GetExtendedTcpTable runs for PID\n" );
  MIB_TCPTABLE_OWNER_PID *tcptable_p;
  DWORD *tcptable_p_size;
  dwstatus = GetExtendedTcpTable (
    ( PVOID ) tcptable_p,
    ( PDWORD ) tcptable_p_size,
    FALSE,
    AF_INET,
    TCP_TABLE_OWNER_PID_CONNECTIONS,
    0
  );
  if ( print_GetExtendedTcpTable_error ( dwstatus ) < 0 ) {
    return -1;
  }
  
  // module
  // printf ( "GetExtendedTcpTable runs for MODULE\n" );
  // MIB_TCPTABLE_OWNER_MODULE *tcptable_m;
  // DWORD *tcptable_m_size;
  // dwstatus = GetExtendedTcpTable (
    // ( PVOID ) tcptable_m,
    // ( PDWORD ) tcptable_m_size,
    // FALSE,
    // AF_INET,
    // TCP_TABLE_OWNER_MODULE_CONNECTIONS,
    // 0
  // );
  // if ( print_GetExtendedTcpTable_error ( dwstatus ) < 0 ) {
    // return -1;
  // }
  
  // pid 6
  printf ( "GetExtendedTcpTable runs for PID6\n" );
  MIB_TCP6TABLE_OWNER_PID *tcp6table_p;
  DWORD *tcp6table_p_size;
  dwstatus = GetExtendedTcpTable (
    ( PVOID ) tcp6table_p,
    ( PDWORD ) tcp6table_p_size,
    FALSE,
    AF_INET6,
    TCP_TABLE_OWNER_PID_CONNECTIONS,
    0
  );
  if ( print_GetExtendedTcpTable_error ( dwstatus ) < 0 ) {
    return -1;
  }
  
  // module 6
  // printf ( "GetExtendedTcpTable runs for MODULE6\n" );
  // MIB_TCP6TABLE_OWNER_MODULE *tcp6table_m;
  // DWORD *tcp6table_m_size;
  // dwstatus = GetExtendedTcpTable (
    // ( PVOID ) tcp6table_m,
    // ( PDWORD ) tcp6table_m_size,
    // FALSE,
    // AF_INET6,
    // TCP_TABLE_OWNER_MODULE_CONNECTIONS,
    // 0
  // );
  // if ( print_GetExtendedTcpTable_error ( dwstatus ) < 0 ) {
    // return -1;
  // }
  
  // GetPerTcp(*)ConnectionEStats
  
  ULONG ulstatus;
  
  // ulstatus = GetPerTcpConnectionEStats (
    // PMIB_TCPROW     Row,
    // TCP_ESTATS_TYPE EstatsType,
    // PUCHAR          Rw,
    // ULONG           RwVersion,
    // ULONG           RwSize,
    // PUCHAR          Ros,
    // ULONG           RosVersion,
    // ULONG           RosSize,
    // PUCHAR          Rod,
    // ULONG           RodVersion,
    // ULONG           RodSize
  // );
  
  // fuck, no GetPerTcpConnectionEStats on windows xp
  
}
