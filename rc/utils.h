#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

typedef struct UTILS_Vector {
  void *array;
  int length;
} UTILS_vector_t;

int
UTILS_vector_init ( UTILS_vector_t *o ) {
  o->array = NULL;
  o->length = 0;
}

int
UTILS_vector_push ( UTILS_vector_t *o, const void *v ) {
  
}


int
UTILS_vector_pop () {

}


#endif // UTILS_H