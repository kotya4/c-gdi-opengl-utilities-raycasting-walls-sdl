#include <stdio.h>
#include "bmp_indexed.h"


int
main () {
  const char filename[] = "web64/ancient.bmp";
  
  int status;
  
  BMP_obj_t bmp;
  status = BMP_read ( &bmp, filename );
  if ( status == -1 ) {
    printf ( "BMP_read: cannot read file %s\n", filename );
    return -1;
  }
  else if ( status < 0 ) {
    printf ( "BMP_read: returns error %d\n", status );
    return -1;
  }
  
  const int max_palette_len = 64; // for test: to fit to base64
  BMP_indexed_t o;
  status = BMP_indexed_init ( &o, &bmp, max_palette_len );
  if ( status == -1 ) {
    printf ( "BMP_init_indexed: provided pallete size was overloaded\n" );
    return -1;
  }
  else if ( status == -2 ) {
    printf ( "BMP_init_indexed: realloc returns NULL\n" );
    return -1;
  }
  else if ( status < 0 ) {
    printf ( "BMP_init_indexed: returns error %d\n", status );
    return -1;
  }
  BMP_kill ( &bmp );
  
  const char dict64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  
  printf ( "\nIndexed BMP (%dx%d):\n\n", o.width, o.height );
  for ( int y = 0; y < o.height; ++y ) {
    for ( int x = 0; x < o.width; ++x ) {
      const int index = o.array[ y * o.width + x ];
      const BMP_rgb24_t color = o.palette.colors[ index ];
      if ( o.palette.length > 64 ) printf( "%03d ", index );
      else printf( "%c", dict64[ index ] );
    }
    printf( "\n" );
  }
  
  printf ( "\nPalette (%d):\n\n", o.palette.length );
  for ( int i = 0; i < o.palette.length; ++i ) {
    const BMP_rgb24_t color = o.palette.colors[ i ];
    printf( "%03d : %02x %02x %02x", i, color.r, color.g, color.b );
    if ( o.palette.length <= 64 ) {
      printf( " [%c]", dict64[ i ] );
    }
    printf("\n");
  }
  
  BMP_indexed_kill ( &o );
  
  return 0;
}
