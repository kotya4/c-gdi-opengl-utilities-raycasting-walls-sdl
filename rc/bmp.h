#ifndef BMP_H
#define BMP_H

#include <stdlib.h> // malloc
#include <stdio.h> // FILE


typedef struct BMP_Obj {
  int length;
  int width;
  int height;
  unsigned char info [ 54 ];
  unsigned char *bgr;
} BMP_obj_t;


int
BMP_read ( BMP_obj_t *o, const char *filename ) {
  FILE *f = fopen ( filename, "rb" );
  if ( f == NULL ) return -1;
  fread ( o->info, sizeof ( unsigned char ), 54, f ); // read the 54-byte header
  o->width = *( int* ) &o->info [ 18 ];
  o->height = *( int* ) &o->info [ 22 ]; // can be -1
  o->length = 3 * o->width * o->height; // allocate 3 bytes per pixel
  o->bgr = malloc ( o->length * sizeof ( unsigned char ) );
  fread ( o->bgr, sizeof ( unsigned char ), o->length, f ); // read the rest of the data at once
  // image is also mirrored
  fclose ( f );
  return 0;
}


void
BMP_kill ( BMP_obj_t *o ) {
  free ( o->bgr );
}


void
BMP_mirror ( BMP_obj_t *o ) {
  for ( int y = 0; y < o->height >> 1; ++y ) {
    for ( int x = 0; x < o->width; ++x ) {
      const int j = o->width * ( o->height - 1 - y ) + x;
      const int k = o->width * y + x;

      const unsigned char b = o->bgr [ j * 3 + 0 ];
      const unsigned char g = o->bgr [ j * 3 + 1 ];
      const unsigned char r = o->bgr [ j * 3 + 2 ];

      o->bgr [ j * 3 + 0 ] = o->bgr [ k * 3 + 0 ];
      o->bgr [ j * 3 + 1 ] = o->bgr [ k * 3 + 1 ];
      o->bgr [ j * 3 + 2 ] = o->bgr [ k * 3 + 2 ];

      o->bgr [ k * 3 + 0 ] = b;
      o->bgr [ k * 3 + 1 ] = g;
      o->bgr [ k * 3 + 2 ] = r;
    }
  }
}


#endif // BMP_H
