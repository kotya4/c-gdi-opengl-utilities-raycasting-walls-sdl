#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "vector.h"


#define max(x, y) ( x > y ? x : y )
#define min(y, x) ( x < y ? x : y )
#define nextpow2_u32( x ) ( 1 << ( 32 - __builtin_clz ( x - 1 ) ) )








#endif // UTILS_H