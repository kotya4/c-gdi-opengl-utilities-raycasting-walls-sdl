#include "bmp_d.h"

int
main () {
  size_t ibmps_length = 0;
  BMP_indexed_t *ibmps = NULL;
  int status = BMP_d ( &ibmps, &ibmps_length );
  if ( status < 0 ) {
    printf("BMP_d: status %d\n", status);
  }
  
  for ( int i = 0; i < ibmps_length; ++i ) BMP_indexed_kill ( &ibmps[ i ] );
  free ( ibmps );
}
