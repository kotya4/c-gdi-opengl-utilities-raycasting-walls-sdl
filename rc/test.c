#include <stdio.h>
#include <math.h>
#include "utils.h"

int
main () {
  
  int a[3] = { 1, 2, 3,  };
  
  printf ( "%d\n", a[2] );
  printf ( "%d\n", ( int ) ceil ( log2 ( 5 ) ) );
  
  
  return 0;
  
  // printf ( "%d test\n", 0b );
  
  vectorbit_t bits = { NULL, 0, 0, 0 };
  
  int status;
  
  status = vectorbit_push ( &bits, 1 ); printf ( "%d ", status );
  status = vectorbit_push ( &bits, 0 ); printf ( "%d ", status );
  status = vectorbit_push ( &bits, 1 ); printf ( "%d ", status );
  status = vectorbit_push ( &bits, 0 ); printf ( "%d ", status );
  status = vectorbit_push ( &bits, 0 ); printf ( "%d ", status );
  status = vectorbit_push ( &bits, 1 ); printf ( "%d ", status );
  status = vectorbit_push ( &bits, 1 ); printf ( "%d ", status );
  status = vectorbit_push ( &bits, 0 ); printf ( "%d ", status );
  status = vectorbit_push ( &bits, 1 ); printf ( "%d ", status );
  status = vectorbit_push ( &bits, 0 ); printf ( "%d ", status );
  status = vectorbit_push ( &bits, 1 ); printf ( "%d ", status );
  status = vectorbit_push ( &bits, 1 ); printf ( "%d ", status );
  status = vectorbit_push ( &bits, 1 ); printf ( "%d ", status );
  status = vectorbit_push ( &bits, 0 ); printf ( "%d ", status );
   printf ( "\n" );
  
  status = vectorbit_pop ( &bits ); printf ( "%d ", status );
  status = vectorbit_pop ( &bits ); printf ( "%d ", status );
  status = vectorbit_pop ( &bits ); printf ( "%d ", status );
  status = vectorbit_pop ( &bits ); printf ( "%d ", status );
  status = vectorbit_pop ( &bits ); printf ( "%d ", status );
  status = vectorbit_pop ( &bits ); printf ( "%d ", status );
  status = vectorbit_pop ( &bits ); printf ( "%d ", status );
  status = vectorbit_pop ( &bits ); printf ( "%d ", status );
  status = vectorbit_pop ( &bits ); printf ( "%d ", status );
  status = vectorbit_pop ( &bits ); printf ( "%d ", status );
  status = vectorbit_pop ( &bits ); printf ( "%d ", status );
  status = vectorbit_pop ( &bits ); printf ( "%d ", status );
  status = vectorbit_pop ( &bits ); printf ( "%d ", status );
  status = vectorbit_pop ( &bits ); printf ( "%d ", status );
  status = vectorbit_pop ( &bits ); printf ( "%d ", status );
   printf ( "\n" );
}