# Includes

ISDL2 = -IC:\SDL2-2.0.14\i686-w64-mingw32\include\SDL2

INCLUDE = $(ISDL2)

# Libraries

LSDL2 = -LC:\SDL2-2.0.14\i686-w64-mingw32\lib

LIBRARY = $(LSDL2)

# Linker

LINKER = -lmingw32 -lSDL2main -lSDL2 -std=c99

# Targets

all:
	gcc main.c $(INCLUDE) $(LIBRARY) $(LINKER) -o rc

d:
	gcc -d main.c $(INCLUDE) $(LIBRARY) $(LINKER) -o rc

test:
	gcc test.c -std=c99 -o test
	./test

bmp_indexed:
	gcc test_bmp_indexed.c -std=c99 -o test_bmp_indexed
	./test_bmp_indexed
	
map:
	gcc test_map.c -std=c99 -o test_map
	./test_map

huffman:
	gcc test_huffman.c -std=c99 -o test_huffman
	./test_huffman
	
bmp_compress:
	gcc bmp_compress.c -std=c99 -o bmp_compress
	./bmp_compress
	