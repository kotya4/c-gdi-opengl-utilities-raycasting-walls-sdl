# Includes

ISDL2 = -IC:\SDL2-2.0.14\i686-w64-mingw32\include\SDL2

INCLUDE = $(ISDL2)

# Libraries

LSDL2 = -LC:\SDL2-2.0.14\i686-w64-mingw32\lib

LIBRARY = $(LSDL2)

# Linker

LINKER = -lmingw32 -lSDL2main -lSDL2 -std=c99

# Targets

BUILDPATH = .

all:
	gcc main.c $(INCLUDE) $(LIBRARY) $(LINKER) -o $(BUILDPATH)/rc
	./rc
	
d:
	gcc -d main.c $(INCLUDE) $(LIBRARY) $(LINKER) -o $(BUILDPATH)/rc -O0 -g

r:
	gcc main.c $(INCLUDE) $(LIBRARY) $(LINKER) -o $(BUILDPATH)/rc -O2 -s -DNDEBUG

imgc:
	gcc imgc.c -std=c99 -o $(BUILDPATH)/imgc
	./imgc
	
test:
	gcc test.c -std=c99 -o $(BUILDPATH)/test
	./test
	
test_bmp_d:
	gcc test_bmp_d.c -std=c99 -o $(BUILDPATH)/test_bmp_d
	./test_bmp_d