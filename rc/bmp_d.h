// Decompresses values from img.h into BMP_indexed_t array
#ifndef BMP_D_H 
#define BMP_D_H

#include <stdint.h> // uint8_t
#include <stdlib.h> // malloc
#include <time.h> // time
#include <math.h> // log2
#include "img.h"
#include "vector.h"
#include "bmp/bmp_indexed.h"


int
BMP_d ( BMP_indexed_t **ibmps, size_t *length ) {
  *length = IMG_FILES_LENGTH;
  *ibmps = malloc ( *length * sizeof **ibmps );
  
  vectorbit_t imagesizes =
    { IMG_imagesizes
    , IMG_IMAGESIZES_LENGTH
    , IMG_IMAGESIZES_LENGTH - 1
    , VECTORBIT_BITSLEN ( IMG_FILES_LENGTH, 4 )
    };
    
  vectorbit_t colorsnum =
    { IMG_colorsnum
    , IMG_COLORSNUM_LENGTH
    , IMG_COLORSNUM_LENGTH - 1
    , VECTORBIT_BITSLEN ( IMG_FILES_LENGTH, 4 )
    };
 
  vectorbit_t images =
    { IMG_images
    , IMG_IMAGES_LENGTH
    , IMG_IMAGES_LENGTH - 1
    , VECTORBIT_BITSLEN ( IMG_FILES_LENGTH, IMG_BITSLENGTH )
    };
 
  #define BMP_D_PARSE_SIZE( s )({  \
    if      ( s == 0b00 ) s = 32;  \
    else if ( s == 0b01 ) s = 64;  \
    else if ( s == 0b10 ) s = 128; \
    else if ( s == 0b11 ) s = 256; \
    else return -1;                \
  })
  
  int colors_index = IMG_COLORS_LENGTH;
  
  for ( int i = 0; i < IMG_FILES_LENGTH; ++i ) {
    BMP_indexed_t *ibmp = &( *ibmps )[ IMG_FILES_LENGTH - i - 1 ];
    BMP_palette_t *palette = &ibmp->palette;
    
    int colors_length = vectorbit_pop_many ( &colorsnum, 4 );
    if ( colors_length < 0 ) return -2;
    colors_length += 1;
    
    palette->length = colors_length;
    palette->colors = malloc ( colors_length * sizeof *palette->colors );
    for ( int k = 0; k < colors_length; ++k ) {
      --colors_index;
      if ( colors_index < 0 ) return -3;
      const uint8_t rgb332 = IMG_colors[ colors_index ];
      const uint8_t r = ( ( rgb332 >> 5 ) & 0b111 ) << 5;
      const uint8_t g = ( ( rgb332 >> 2 ) & 0b111 ) << 5;
      const uint8_t b = ( ( rgb332 >> 0 ) & 0b11  ) << 6;
      palette->colors[ colors_length - k - 1 ].r = r;
      palette->colors[ colors_length - k - 1 ].g = g;
      palette->colors[ colors_length - k - 1 ].b = b;
    }
    
    ibmp->height = vectorbit_pop_many ( &imagesizes, 2 );
    ibmp->width  = vectorbit_pop_many ( &imagesizes, 2 );
    BMP_D_PARSE_SIZE ( ibmp->width );
    BMP_D_PARSE_SIZE ( ibmp->height );
    
    ibmp->length = ibmp->width * ibmp->height;
    ibmp->array = malloc ( ibmp->length * sizeof *ibmp->array );
    
    const int indexcap = ( int ) ceil ( log2 ( palette->length ) );
    
    for ( int k = 0; k < ibmp->length; ++k ) {
      const int index = vectorbit_pop_many ( &images, indexcap );
      if ( 0 > index || index >= palette->length ) { 
        printf ( ">> index: %d\n", index );
        return -4;
        
      }
      ibmp->array[ ibmp->length - k - 1 ] = index;
    }
    
    // printf ( "i: %02d ", i );
    // printf ( "indexcap: %d ", indexcap );
    // printf ( "wxh: %d %d ", ibmp->width, ibmp->height );
    // printf ( "colors len: %d\n", colors_length );
  }
}

#endif // BMP_D_H
