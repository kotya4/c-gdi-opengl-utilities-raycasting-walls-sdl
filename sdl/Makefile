INCLUDE = -IC:\SDL2-2.0.14\i686-w64-mingw32\include -IC:\glew-2.1.0\include

LIBRARY = -LC:\SDL2-2.0.14\i686-w64-mingw32\lib -LC:\glew-2.1.0\lib

LINKER = -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglu32 -lglew32

all:
	gcc main.c $(INCLUDE) $(LIBRARY) $(LINKER) -o sdl
