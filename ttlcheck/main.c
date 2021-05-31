#include <windows.h>
#include <stdio.h>

int
main () {
  HKEY hKey;
  LONG status;
  
  status = RegOpenKeyEx (
    HKEY_LOCAL_MACHINE,
    "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters",
    0,
    KEY_READ,
    &hKey
  );
  
  if ( status != ERROR_SUCCESS ) {
    printf ( "RegOpenKeyEx: returns an error %d\n", status );
    return -1;
  }
  
  DWORD value;
  DWORD value_size = sizeof value;
  
  status = RegQueryValueEx (
    hKey,
    "DefaultTTL",
    NULL,
    NULL,
    ( LPBYTE ) ( &value ),
    ( LPDWORD ) ( &value_size )
  );
  
  if ( status == ERROR_FILE_NOT_FOUND ) {
    printf ( "RegQueryValueEx: ERROR_FILE_NOT_FOUND\n" );
    return -1;
  }
  
  if ( status != ERROR_SUCCESS ) {
    printf ( "RegQueryValueEx: returns an error %d\n", status );
    return -1;
  }
  
  printf ( "DefaultTTL %d\n", value );
  
  char value_str[ 16 ];
  sprintf ( value_str, "%d", value );
  
  MessageBox (
    NULL,
    value_str,
    "DefaultTTL",
    MB_OK
  );
  
  status = RegCloseKey (
    hKey
  );
  
  if ( status != ERROR_SUCCESS ) {
    printf ( "RegCloseKey: returns an error %d\n", status );
    return -1;
  }
  
  return 0;
}
