#include <stdio.h>
#include "map.h"


void
MAP_print ( const MAP_map_t *o ) {
  for ( int y = 0; y < o->height; ++y ) {
    for ( int x = 0; x < o->width; ++x ) {
      if ( o->array[y * o->width + x] == 0 ) {
        printf ( " " );
      }
      else if ( o->array[y * o->width + x] == 1 ) {
        printf ( "#" );
      }
      else if ( o->array[y * o->width + x] == 2 ) {
        printf ( "." );
      }
      else {
        printf ( "%d", o->array[y * o->width + x] );
      }
    }
    printf ( "\n" );
  }
}


int
main () {
  // const int seed = 1622580154; // time ( NULL );
  const int seed = time ( NULL );
  srand ( seed );
  printf ( "seed (unixtime): %d\n", seed );

  MAP_map_t map;
  MAP_gen ( &map );
  MAP_print ( &map );
  
  MAP_kill ( &map );
}