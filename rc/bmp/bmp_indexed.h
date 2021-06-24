#ifndef BMP_INDEXED_H
#define BMP_INDEXED_H

#include <stdbool.h>
#include "bmp.h"


typedef struct BMP_RGB24 {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} BMP_rgb24_t;


typedef struct BMP_Palette {
  BMP_rgb24_t *colors;
  int length;
} BMP_palette_t;


typedef struct BMP_Indexed {
  BMP_palette_t palette;
  int *array;
  int width;
  int height;
  int length;
} BMP_indexed_t;


void
BMP_indexed_kill ( BMP_indexed_t *o ) {
  free ( o->array );
  o->array = NULL;
  free ( o->palette.colors );
  o->palette.colors = NULL;
}


#define BMP_RGB24_EQUAL( a, c ) ( a.r == c.r && a.g == c.g && a.b == c.b )


// max_palette_len ignored if <= 0
int
BMP_indexed_init ( BMP_indexed_t *o, const BMP_obj_t *bmp, const int max_palette_len ) {
  o->width = bmp->width;
  o->height = bmp->height;
  o->length = o->width * o->height;
  o->array = malloc ( o->length * sizeof *o->array );
  const int palette_step_len = 16;
  o->palette.length = 0;
  o->palette.colors = NULL;
  int *array_ptr = o->array;
  int palette_len = 0;
  for ( int i = 0; i < bmp->length; i += 3 ) {
    const BMP_rgb24_t color = { bmp->bgr[ i + 2 ], bmp->bgr[ i + 1 ], bmp->bgr[ i + 0 ] };
    int index; // find same color in palette
    bool found = false;
    for ( index = 0; index < palette_len; ++index ) {
      if ( BMP_RGB24_EQUAL ( o->palette.colors[ index ], color ) ) {
        found = true;
        break;
      }
    }
    if ( !found ) {
      if ( max_palette_len > 0 && palette_len + 1 > max_palette_len ) {
        BMP_indexed_kill ( o );
        return -1; // pallete max size less than index
      }
      if ( o->palette.length <= palette_len ) { // expands palette
        o->palette.length += palette_step_len;
        void *oldptr = o->palette.colors;
        o->palette.colors = realloc (
          o->palette.colors,
          o->palette.length * sizeof *o->palette.colors
        );
        if ( !o->palette.colors ) {
          BMP_indexed_kill ( o );
          free ( oldptr );
          return -2; // realloc error
        }
      }
      // append color
      index = palette_len;
      o->palette.colors[ index ] = color;
      ++palette_len;
    }
    // append index
    ( *array_ptr ) = index;
    ++array_ptr;
  }
  if ( o->palette.length != palette_len ) { // realloc
    o->palette.length = palette_len;
    void *oldptr = o->palette.colors;
    o->palette.colors = realloc (
      o->palette.colors,
      o->palette.length * sizeof *o->palette.colors
    );
    if ( !o->palette.colors ) {
      BMP_indexed_kill ( o );
      free ( oldptr );
      return -2; // realloc error
    }
  }
  return 0;
}


int
BMP_indexed_rgb_at ( const BMP_indexed_t *o, size_t x, size_t y ) {
  if ( x >= o->width || y >= o->height ) return 0xff00ff; // pink then err
  const size_t index = o->array[ y * o->width + x ];
  const BMP_rgb24_t rgb = o->palette.colors[ index ];
  return ( rgb.r << 0x10 ) | ( rgb.g << 0x08 ) | ( rgb.b << 0x00 );
}

BMP_rgb24_t *
BMP_indexed_rgb24_at ( const BMP_indexed_t *o, size_t x, size_t y ) {
  if ( x >= o->width || y >= o->height ) return NULL;
  const size_t index = o->array[ y * o->width + x ];
  return &o->palette.colors[ index ];
}

#endif // BMP_INDEXED_H
