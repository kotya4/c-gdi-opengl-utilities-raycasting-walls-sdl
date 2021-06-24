#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h> // uint8_t
#include <stdlib.h> // realloc


typedef struct VectorU8 {
  uint8_t *array;
  size_t length;
  size_t capacity;
} vectoru8_t;


#define VECTORU8_STEP 16


int
vectoru8_push ( vectoru8_t *v, uint8_t value ) {
  if ( v == NULL ) return -1;
  if ( v->array == NULL ) {
    v->length = 0;
    v->capacity = 0;
  }
  if ( v->length + 1 > v->capacity ) {
    void *oldptr = v->array;
    v->array = realloc ( v->array, ( v->capacity + VECTORU8_STEP ) * sizeof *v->array );
    if ( !v->array ) {
      v->array = oldptr;
      return -2;
    }
    v->capacity += VECTORU8_STEP;
  }
  v->array[ v->length ] = value;
  ++v->length;
  return 0;
}



typedef struct VectorBit {
  uint8_t *array;
  size_t capacity; // number of allocated bytes
  size_t byteindex; // index of byte in use
  size_t bitslength; // number of pushed bits in last byte
} vectorbit_t;


#define VECTORBIT_STEP 1


// calculates " bitslength " by number of values aka
// " size " and number of bits per value aka " bits ".
#define VECTORBIT_BITSLEN( size, bits ) ( size * bits % 8 ? size * bits % 8 : 8 )


int
vectorbit_push ( vectorbit_t *v, uint8_t value ) {
  if ( v == NULL ) return -1;
  if ( v->array == NULL ) {
    v->capacity = 0;
    v->byteindex = 0;
    v->bitslength = 0;
  }
  if ( v->bitslength + 1 > 8 ) {
    // number of pushed bits in byte bigger than one byte can store
    v->byteindex += 1;
    v->bitslength = 0;
  }
  if ( v->byteindex == v->capacity ) {
    // allocate one more byte
    void *oldptr = v->array;
    v->array = realloc ( v->array, ( v->capacity + 1 ) * sizeof *v->array );
    if ( !v->array ) {
      v->array = oldptr;
      return -2;
    }
    v->capacity += 1;
    // zero allocated memory
    v->array[ v->byteindex ] = 0;
  }
  // store bit in array
  v->array[ v->byteindex ] |= ( ( value & 1 ) << v->bitslength );
  ++v->bitslength;
  return 0;
}


int
vectorbit_push_many ( vectorbit_t *v, size_t value, size_t num ) {
  // TODO: rewrite with precalculated reallocation
  int status;
  for ( int i = 0; i < num; ++i ) {
    status = vectorbit_push ( v, ( value >> i & 1 ) );
    if ( status < 0 ) return status;
  }
  return 0;
}


// returns 0 or 1 on success or <0 if any error occures
int
vectorbit_pop ( vectorbit_t *v ) {
  if ( v == NULL ) return -1;
  if ( v->array == NULL ) return -2;
  if ( v->bitslength == 0 ) {
    if ( v->byteindex == 0 ) return -3;
    --v->byteindex;
    v->bitslength = 8;
  }
  --v->bitslength;
  int value = v->array[ v->byteindex ] >> v->bitslength & 1;
  // v->array[ v->byteindex ] ^= value << v->bitslength; // flush
  return value;
}


int
vectorbit_pop_many ( vectorbit_t *v, size_t num ) {
  int value = 0;
  for ( int i = 0; i < num; ++i ) {
    const int ret = vectorbit_pop ( v );
    if ( ret < 0 ) return ret;
    value |= ret << ( num - i - 1 );
  }
  return value;
}



#endif // VECTOR_H