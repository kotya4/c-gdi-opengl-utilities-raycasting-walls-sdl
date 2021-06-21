#include "huffman.h"
#include <stdlib.h> // rand, malloc
#include <stdio.h> // printf
#include <time.h> // time
#include <math.h> // log2


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
  
  
  srand ( time ( NULL ) );
  // has to be strongly indexed ( each index have to have at least one entry )
  #define DATA_SIZE 1000
  #define DATA_CAP  32
  int data [ DATA_SIZE ];
  for ( int i = 0; i < DATA_SIZE; ++i ) {
    if ( i < DATA_CAP ) data[ i ] = i;
    else data[ i ] = rand () % DATA_CAP;
  }
  #define DATA_PATCH 1
  #if DATA_PATCH == 1
  // fills half of data with same index
  for ( int i = DATA_SIZE >> 1; i < DATA_SIZE; ++i ) {
    data[ i ] = DATA_CAP - 1;
  }
  #endif
  #undef DATA_PATCH
  
  
  #define SHOW_DATA 0
  #if SHOW_DATA == 1
  printf ( "\ndata :\n" );
  for ( int i = 0; i < DATA_SIZE; ++i ) {
    printf ( "%02d ", data[ i ] );
  }
  #endif
  #undef SHOW_DATA
  
  
  int status;
  HUFFMAN_heap_t heap;
  
  status = HUFFMAN_init_heap ( &heap, data, DATA_SIZE );
  if ( status == -1 ) {
    printf ( "HUFFMAN_init_heap: realloc returns NULL\n" );
  }
  else if ( status < 0 ) {
    printf ( "HUFFMAN_init_heap: returns error %d\n", status );
  }
  
  printf ( "\n\ndict ( value : freq ) :\n" );
  for ( int i = 0; i < heap.length; ++i ) {
    printf ( "  %02d : %d\n", heap.array[ i ].value, heap.array[ i ].freq );
  }
  
  int dict_len;
  
  status = HUFFMAN_build_heap ( &heap, &dict_len );
  if ( status == -1 ) {
    printf ( "HUFFMAN_build_heap: realloc returns NULL\n" );
  }
  else if ( status == -2 ) {
    printf ( "HUFFMAN_build_heap: min freq not found\n" );
  }
  else if ( status < 0 ) {
    printf ( "HUFFMAN_build_heap: returns error %d\n", status );
  }
  
  
  #define SHOW_HEAP 0
  #if SHOW_HEAP == 1
  printf ( "\nheap:\n" );
  for ( int i = 0; i < heap.length; ++i ) {
    printf ( "  index: %d\n", i );
    printf ( "    freq: %d\n", heap.array[ i ].freq );
    printf ( "    has parent: %d\n", !!heap.array[ i ].parent );
    printf ( "    has left: %d", !!heap.array[ i ].left );
    if ( !!heap.array[ i ].left )
      printf ( " ( i: %d )", heap.array[ i ].left->index );
    printf ( "\n" );
    printf ( "    has right: %d", !!heap.array[ i ].right );
    if ( !!heap.array[ i ].right )
      printf ( " ( i: %d )", heap.array[ i ].right->index );
    printf ( "\n" );
    printf ( "    value: %02d\n", heap.array[ i ].value );
  }
  #endif
  #undef SHOW_HEAP
  
  
  HUFFMAN_dict_t *dict = malloc ( dict_len * sizeof *dict );
  // for test only. if any value become < 0 after HUFFMAN_make_dict
  // then data is probably not strongly indexed (tested further)
  {
    for ( int i = 0; i < dict_len; ++i ) dict[ i ].code = -1;
  }
  HUFFMAN_make_dict ( &heap.array[ heap.length - 1 ], dict, 0, 0 );
  HUFFMAN_kill_heap ( &heap );
  
  
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
  
  #define nextpow2_u32( x ) ( 1 << ( 32 - __builtin_clz ( x - 1 ) ) )
  
  const int data_bpw = log2 ( nextpow2_u32 ( DATA_CAP ) );
  const int data_size = data_bpw * DATA_SIZE;
  const int data_size_b = ( int ) ceil ( data_size / 8.0 );
  printf ( "\nraw data size: %d bits ( %d bits per word )\n", data_size, data_bpw );
  printf ( " ~ %d bytes\n", data_size_b );
  
  const int size_bits = log2 ( nextpow2_u32 ( max_dict_size ) );
  // terminator is the duplicate of smallest table value ( with size ).
  const int terminator_bits = dict[ min_dict_i ].size + size_bits;
  const int table_size_bits = dict_size_sum + size_bits * dict_len + terminator_bits;
  int encoded_size = 0;
  for ( int i = 0; i < DATA_SIZE; ++i ) {
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
  
  
  
  
  
  free ( strbuff );
  free ( table );
  free ( dict );
}
