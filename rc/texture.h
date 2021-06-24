#ifndef TEXTURE_H
#define TEXTURE_H
#include <stdbool.h>
#include "bmp/bmp_indexed.h"


typedef struct {
  BMP_indexed_t *ibmp;
  bool transparent;
} texture_t;


#endif // TEXTURE_H