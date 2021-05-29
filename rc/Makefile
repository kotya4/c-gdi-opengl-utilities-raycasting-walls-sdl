# Includes

ISDL2 = -IC:\SDL2-2.0.14\i686-w64-mingw32\include\SDL2

INCLUDE = $(ISDL2)

# Libraries

LSDL2 = -LC:\SDL2-2.0.14\i686-w64-mingw32\lib

LIBRARY = $(LSDL2)

# Linker

LINKER = -lmingw32 -lSDL2main -lSDL2 -std=c99

# Make

all:
	gcc main.c $(INCLUDE) $(LIBRARY) $(LINKER) -o rc

d:
	gcc -d main.c $(INCLUDE) $(LIBRARY) $(LINKER) -o rc


test:
	gcc test.c -std=c99 -o test
