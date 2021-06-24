#ifndef BMP_font_H
#define BMP_font_H

#include <stdlib.h> // malloc
#include "bmp.h" // BMP_obj_t


typedef struct BMP_Font {
  BMP_obj_t bmpobj;
  int cw; // pixels in character
  int ch;
  int tw; // characters in table
  int th;
  unsigned char **table;
} BMP_font_t;


int
BMP_font_init ( BMP_font_t *f, const char *filepath ) {
  if ( BMP_read ( &f->bmpobj, filepath ) < 0 ) {
    return -1; // File cannot be opened
  }
  BMP_mirror ( &f->bmpobj );
  const int W = f->bmpobj.width; // pixels in table
  const int H = f->bmpobj.height;
  const int M = 16; // characters in table
  const int N = 16;
  const int X = W / M; // pixels in character
  const int Y = H / N;
  f->cw = X;
  f->ch = Y;
  f->tw = M;
  f->th = N;
  f->table = malloc ( M * N * sizeof ( unsigned char* ) );
  for ( int n = 0; n < N; ++n ) {
    for ( int m = 0; m < M; ++m ) {
      const int ci = M * n + m;
      f->table [ ci ] = malloc ( X * Y * sizeof ( unsigned char ) );
      for ( int y = 0; y < Y; ++y ) {
        for ( int x = 0; x < X; ++x ) {
          const int bi = ( W * Y * n + X * m + W * y + x ) * 3; // 3 bytes per pixel
          const unsigned char b = f->bmpobj.bgr [ bi + 0 ];
          const unsigned char g = f->bmpobj.bgr [ bi + 1 ];
          const unsigned char r = f->bmpobj.bgr [ bi + 2 ];
          f->table [ ci ] [ X * y + x ] = b || g || r;
        }
      }
    }
  }
  return 0;
}


void
BMP_font_kill ( BMP_font_t *f ) {
  for ( int i = 0; i < f->tw * f->th; ++i ) free ( f->table [ i ] );
  free ( f->table );
  BMP_kill ( &f->bmpobj );
}


#endif // BMP_font_H
