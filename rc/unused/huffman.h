#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdbool.h> // bool
#include <stdlib.h> // realloc


// HINT: strongly indexate data values before build
typedef struct HUFFMAN_Tree {
  int index;
  struct HUFFMAN_Tree *parent;
  struct HUFFMAN_Tree *right;
  struct HUFFMAN_Tree *left;
  int value;
  int freq;
} HUFFMAN_tree_t;


typedef struct HUFFMAN_Heap {
  HUFFMAN_tree_t *array;
  int length;
} HUFFMAN_heap_t;


typedef struct HUFFMAN_Dict {
  int size;  // length of code in bits
  int code;  // huffman code
} HUFFMAN_dict_t;


void
HUFFMAN_kill_heap ( HUFFMAN_heap_t *o ) {
  if ( o->array ) free ( o->array );
}


int
HUFFMAN_init_heap ( HUFFMAN_heap_t *o, const int *data, const int data_length ) {
  const int realloc_size = 16;
  o->array = NULL; // must be null to use in realloc
  o->length = 0;
  int heap_size = 0;
  for ( int i = 0; i < data_length; ++i ) {
    bool found = false;
    for ( int k = 0; k < o->length; ++k ) {
      if ( data[ i ] == o->array[ k ].value ) { // found
        ++o->array[ k ].freq;
        found = true;
        break;
      }
    }
    if ( !found ) { // append
      if ( o->length >= heap_size ) { // realloc
        heap_size += realloc_size;
        void *oldptr = ( void * ) o->array;
        o->array = realloc ( o->array, heap_size * sizeof *o->array );
        if ( !o->array ) {
          free ( oldptr );
          return -1;
        }
      }
      o->array[ o->length ].value = data[ i ];      
      o->array[ o->length ].freq = 1;
      ++o->length;
    }
  }
  return 0;
}


int
HUFFMAN_build_heap ( HUFFMAN_heap_t *o, int *dict_len ) {
  *dict_len = o->length;
  // realloc with parents
  o->length = o->length * 2 - 1;
  void *oldptr = o->array;
  o->array = realloc (  o->array, o->length * sizeof *o->array );
  if ( !o->array ) {
    free ( oldptr );
    return -1;
  }
  // init trees
  for ( int i = 0; i < o->length; ++i ) {
    o->array[ i ].index = i;
    o->array[ i ].parent = NULL;
    o->array[ i ].right = NULL;
    o->array[ i ].left = NULL;
    if ( i >= *dict_len ) {
      o->array[ i ].value = -1;
      o->array[ i ].freq = -1;
    }
  }
  // branch minimum leafs
  for ( int hl = *dict_len; hl < o->length; ++hl ) {
    int left_i;
    int right_i;
    unsigned long long min_freq;
    right_i = -1;
    min_freq = ( unsigned long long ) -1;
    for ( int i = 0; i < hl; ++i ) {
      if ( o->array[ i ].parent ) continue;
      if ( min_freq > o->array[ i ].freq ) {
        min_freq = o->array[ i ].freq;
        right_i = i;
      }
    }
    left_i = -1;
    min_freq = ( unsigned long long ) -1;
    for ( int i = 0; i < hl; ++i ) {
      if ( o->array[ i ].parent || i == right_i ) continue;
      if ( min_freq > o->array[ i ].freq ) {
        min_freq = o->array[ i ].freq;
        left_i = i;
      }
    }
    if ( left_i < 0 || right_i < 0 ) return -2;
    o->array[ left_i ].parent = &o->array[ hl ];
    o->array[ right_i ].parent = &o->array[ hl ];
    o->array[ hl ].left = &o->array[ left_i ];
    o->array[ hl ].right = &o->array[ right_i ];
    o->array[ hl ].freq = o->array[ left_i ].freq + o->array[ right_i ].freq;
  }
  return 0;
}


int
HUFFMAN_reverse_bits ( const int v, const int len ) {
  int r = 0;
  for ( int i = 0; i < len; ++i ) r |= ( v >> i & 1 ) << ( len - i - 1 );
  return r;
}


// code and depth must be 0 on first call
void
HUFFMAN_make_dict ( const HUFFMAN_tree_t *o, HUFFMAN_dict_t *dict, int code, int depth ) {
  if ( o->value >= 0 ) {
    dict[ o->value ].size = depth; // length of bits in code
    dict[ o->value ].code = HUFFMAN_reverse_bits ( code, depth );
    return;
  }
  if ( o->left ) HUFFMAN_make_dict ( o->left, dict, code << 1 | 0, depth + 1 );
  if ( o->right ) HUFFMAN_make_dict ( o->right, dict, code << 1 | 1, depth + 1 );
}

/*
int
HUFFMAN_table_length ( const HUFFMAN_dict_t *dict, const int dict_len, int *max_dict_size ) {
  *max_dict_size = 0;
  for ( int i = 0; i < dict_len; ++i ) {
    if ( *max_dict_size < dict[ i ].size ) {
      *max_dict_size = dict[ i ].size;
    }
  }
  return 2 << *max_dict_size - 1;
}
*/

#endif // HUFFMAN_H