#include <string.h> // strncat strlen
#include <stdio.h> // printf fopen
#include <stdlib.h> // malloc realloc
#include <math.h> // log2

#include "bmp.h"
#include "bmp_indexed.h"
#include "huffman.h"


//
// PRELUDE
//


int
stringify_bits ( char *s, const int slen, int v, const int vlen ) {
  for ( int i = 0; i < slen - 1; ++i ) s[ i ] = ' ';
  s[ slen - 1 ] = '\0';
  for ( int i = 0; i < slen - 1; ++i ) {
    if ( i >= vlen ) return i;
    s[ slen - i - 2 ] = '0' + ( v & 1 );
    v = v >> 1;
  }
  return -1;
}


int
main () {
  
  
  //
  // ACT 1. MAKE DATA READY
  //
  
  
  #define FILEPATH "bmp_compress_files.txt"
  FILE *f = fopen ( FILEPATH, "r" );
  if ( !f ) {
    printf ( "fopen: cannot open file %s\n", FILEPATH );
    return -1;
  }
  #define REALLOC_SIZE 16
  int files_num = 0;
  char **files = NULL;
  for ( int i = 0; ; ++i ) {
    if ( i >= files_num ) {
      files_num += REALLOC_SIZE;
      void *oldptr = files;
      files = realloc ( files, files_num * sizeof *files );
      if ( !files ) {
        printf ( "realloc: returns NULL on index %d, files_num %d\n", i, files_num );
        for ( --i; i >= 0; --i ) free ( ( ( void ** ) oldptr )[ i ] );
        free ( oldptr );
        return -1;
      }
    }
    #define STR_LEN 64
    files[ i ] = malloc ( STR_LEN * sizeof **files );
    if ( fgets ( files[ i ], STR_LEN, f ) == NULL ) {
      files_num = i;
      break;
    }
  }
  fclose ( f );
  
  
  int colors_len = 0;
  BMP_rgb24_t *colors = NULL;
  
  int data_len = 0;
  int *data = NULL;
  
  int *p_lens = malloc ( files_num * sizeof *p_lens );
  int *d_ws = malloc ( files_num * sizeof *d_ws );
  int *d_hs = malloc ( files_num * sizeof *d_hs );
  
  
  #define FOLDER "web64/"
  for ( int i = 0; i < files_num; ++i ) {
    
    // concat folder
    
    files[ i ][ strlen( files[ i ] ) - 1 ] = '\0'; // after " fgets " last character is 0xa
    #define PATH_LEN STR_LEN << 1
    char path[ PATH_LEN ] = FOLDER;
    strncat ( path, files[ i ], PATH_LEN );
    // free ( files[ i ] ); does it after log
    
    int status;
    
    // read bmp
    
    BMP_obj_t bmp;
    status = BMP_read ( &bmp, path );
    if ( status == -1 ) {
      printf ( "BMP_read: cannot read file %s\n", path );
      continue;
    }
    else if ( status < 0 ) {
      printf ( "BMP_read: returns error %d\n", status );
      continue;
    }
    
    BMP_mirror ( &bmp );
    
    // indexate bmp
    
    BMP_indexed_t ibmp;
    status = BMP_indexed_init ( &ibmp, &bmp, -1 );
    BMP_kill ( &bmp );
    if ( status == -1 ) {
      printf ( "BMP_init_indexed: provided pallete size was overloaded\n" );
      continue;
    }
    else if ( status == -2 ) {
      printf ( "BMP_init_indexed: realloc returns NULL\n" );
      continue;
    }
    else if ( status < 0 ) {
      printf ( "BMP_init_indexed: returns error %d\n", status );
      continue;
    }
    
    // dump
    
    void *oldptr;
    
    colors_len += ibmp.palette.length;
    oldptr = colors;
    colors = realloc ( colors, colors_len * sizeof *colors );
    if ( !colors ) {
      printf ( "colors::realloc: returns NULL on length %d\n", colors_len );
      free ( data );
      free ( oldptr );
      BMP_indexed_kill ( &ibmp );
      continue;
    }
    BMP_rgb24_t *patelleptr = ibmp.palette.colors;
    for ( int i = colors_len - ibmp.palette.length; i < colors_len; ++i ) {
      colors[ i ] = *patelleptr;
      ++patelleptr;
    }
    
    data_len += ibmp.length;
    oldptr = data;
    data = realloc ( data, data_len * sizeof *data );
    if ( !data ) {
      printf ( "data::realloc: returns NULL on length %d\n", data_len );
      free ( colors );
      free ( oldptr );
      BMP_indexed_kill ( &ibmp );
      continue;
    }
    int *dataptr = ibmp.array;
    for ( int i = data_len - ibmp.length; i < data_len; ++i ) {
      data[ i ] = *dataptr;
      ++dataptr;
    }
    
    p_lens[ i ] = ibmp.palette.length;
    d_ws[ i ] = ibmp.width;
    d_hs[ i ] = ibmp.height;
    
    BMP_indexed_kill ( &ibmp );
  }
  
  
  #if 0 == 1 // SHOW FILE LOG
  for ( int i = 0; i < files_num; ++i ) {
    printf ( "\n" );
    printf ( "file: %s\n", files[ i ] );
    printf ( "size: %d x %d\n", d_ws[ i ], d_hs[ i ] );
    printf ( "palette: %d\n", p_lens[ i ] );
  }
  #endif
  
  
  printf ( "\n" );
  printf ( "data length: %d\n", data_len );
  printf ( "palette length: %d\n", colors_len );
  
  
  //
  // ACT 2. MAKE HUFFMAN READY
  //
  

  int status;
  HUFFMAN_heap_t heap;
  
  status = HUFFMAN_init_heap ( &heap, data, data_len );
  if ( status == -1 ) {
    printf ( "HUFFMAN_init_heap: realloc returns NULL\n" );
    goto huffman_exit_1;
  }
  else if ( status < 0 ) {
    printf ( "HUFFMAN_init_heap: returns error %d\n", status );
    goto huffman_exit_1;
  }

  
  printf ( "\n\ndict ( value : freq ) :\n" );
  for ( int i = 0; i < heap.length; ++i ) {
    printf ( "  %02d : %d\n", heap.array[ i ].value, heap.array[ i ].freq );
  }
  
  int dict_len;
  
  status = HUFFMAN_build_heap ( &heap, &dict_len );
  if ( status == -1 ) {
    printf ( "HUFFMAN_build_heap: realloc returns NULL\n" );
    goto huffman_exit_2;
  }
  else if ( status == -2 ) {
    printf ( "HUFFMAN_build_heap: min freq not found\n" );
    goto huffman_exit_2;
  }
  else if ( status < 0 ) {
    printf ( "HUFFMAN_build_heap: returns error %d\n", status );
    goto huffman_exit_2;
  }
  
  
  HUFFMAN_dict_t *dict = malloc ( dict_len * sizeof *dict );
  // for test only. if any value become < 0 after HUFFMAN_make_dict
  // then data is probably not strongly indexed (tested further)
  {
    for ( int i = 0; i < dict_len; ++i ) dict[ i ].code = -1;
  }
  HUFFMAN_make_dict ( &heap.array[ heap.length - 1 ], dict, 0, 0 );
  
  
  int min_dict_i = 0;
  int max_dict_size = 0;
  int dict_size_sum = 0;
  for ( int i = 0; i < dict_len; ++i ) {
    dict_size_sum += dict[ i ].size;
    if ( dict[ min_dict_i ].size > dict[ i ].size ) min_dict_i = i;
    if ( max_dict_size < dict[ i ].size ) max_dict_size = dict[ i ].size;
  }
  
  const int table_len = 2 << max_dict_size - 1;
  int *table = malloc ( table_len * sizeof *table );
  for ( int i = 0; i < table_len; ++i ) table[ i ] = -1;
  for ( int i = 0; i < dict_len; ++i ) {
    table[ dict[ i ].code ] = i;
  }
  
  printf ( "\nfast-table length : %d\n", table_len );

  // TIP: " dict[ i ].code " must be read that <- way 
  printf ( "\ndict ( value : code : fast-table : length ) :\n" );
  char *strbuff = malloc ( max_dict_size + 1 * sizeof *strbuff );
  for ( int val = 0; val < dict_len; ++val ) {
    // " val " is indexed value needed to be decode from huffman code.
    // table and dict are reverse lists: table[dict[i].code] = i; dict[table[i]].code = i;
    printf ( "  %02d : ", val );
    if ( dict[ val ].code < 0 ) {
      printf ( "incorrect code ( %d )\n", dict[ val ].code );
    } else {
      // dictcode ( zero bits are space )
      stringify_bits ( strbuff, max_dict_size + 1, dict[ val ].code, dict[ val ].size );
      printf ( "%s : ", strbuff );
      printf ( "%02d ", table[ dict[ val ].code ] );
      printf ( "(%s) : %d\n", table[ dict[ val ].code ] == val ? "OK" : "ER", dict[ val ].size );
    }
  }
  
  // DATA_CAP is the maximum value in data ( assume 4 )
  #define DATA_CAP 4
  
  #define nextpow2_u32( x ) ( 1 << ( 32 - __builtin_clz ( x - 1 ) ) )
  
  const int data_bpw = log2 ( nextpow2_u32 ( DATA_CAP ) );
  const int data_size = data_bpw * data_len;
  const int data_size_b = ( int ) ceil ( data_size / 8.0 );
  printf ( "\nraw data size: %d bits ( %d bits per word )\n", data_size, data_bpw );
  printf ( " ~ %d bytes\n", data_size_b );
  
  const int size_bits = log2 ( nextpow2_u32 ( max_dict_size ) );
  // terminator is the duplicate of smallest table value ( with size ).
  const int terminator_bits = dict[ min_dict_i ].size + size_bits;
  const int table_size_bits = dict_size_sum + size_bits * dict_len + terminator_bits;
  int encoded_size = 0;
  for ( int i = 0; i < data_len; ++i ) {
    encoded_size += dict[ data[ i ] ].size;
  }
  const int total_size = encoded_size + table_size_bits;
  const int total_size_b = ( int ) ceil ( total_size / 8.0 );
  printf ( "\nencoded data size: %d bits\n", encoded_size );
  printf ( " + %d bits ( table size )\n", table_size_bits );
  printf ( " = %d bits\n", total_size );
  printf ( " ~ %d bytes\n", total_size_b );
  printf ( "\ncompression rate (bits) : %d %%\n", 100 - 100 * total_size / data_size );
  printf ( "\ncompression rate (bytes) : %d %%\n", 100 - 100 * total_size_b / data_size_b );
  
  
  //
  // ACT 3. ENCODE
  //
  
  
  // HACK: hardcoded data cap
  unsigned char *filestream = calloc ( data_size_b, sizeof *filestream );
  int fs_bits_used = 0;
  unsigned char buffer = 0;
  
  // byte:[ data:[[3:1][3:0]] data:[[2:1][2:0]] data:[[1:1][1:0]] data:[[0:1][0:0]] ]
  
  for ( int i = 0; i < data_len; ++i ) {
    buffer |= ( data[ i ] >> 0 & 1 ) << fs_bits_used;
    ++fs_bits_used;
    if ( fs_bits_used == 8 ) {
      fs_bits_used = 0;
      *filestream = buffer;
      ++filestream;
    }
    buffer |= ( data[ i ] >> 1 & 1 ) << fs_bits_used;
    ++fs_bits_used;
    if ( fs_bits_used == 8 ) {
      fs_bits_used = 0;
      *filestream = buffer;
      ++filestream;
    }
  }
  
  
  
  
  
  free ( filestream );
  
  
  //
  // ACT 4. DEALOCATION
  //
  
  
  free ( strbuff );
  free ( table );
  free ( dict );
  
huffman_exit_2:
  HUFFMAN_kill_heap ( &heap );
  
huffman_exit_1:
  for ( int i = 0; i < files_num; ++i ) free ( files[ i ] );
  free ( files );
  free ( colors );
  free ( data );
  free ( p_lens );
  free ( d_ws );
  free ( d_hs );
  return 0;
}
