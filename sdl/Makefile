# Includes

ISDL2 = -IC:\SDL2-2.0.14\i686-w64-mingw32\include\SDL2
IGLEW = -IC:\glew-2.1.0\include
ISDL2TTF = -IC:\SDL2_ttf-2.0.15\i686-w64-mingw32\include\SDL2

INCLUDE = $(ISDL2) $(IGLEW) $(ISDL2TTF)

# Libraries

LSDL2 = -LC:\SDL2-2.0.14\i686-w64-mingw32\lib
LGLEW = -LC:\glew-2.1.0\lib
LSDL2TTF = -LC:\SDL2_ttf-2.0.15\i686-w64-mingw32\lib

LIBRARY = $(LSDL2) $(LGLEW) $(LSDL2TTF)

# Linker

LINKER = -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lopengl32 -lglu32 -lglew32 -std=c99

# Make

all:
	gcc main.c $(INCLUDE) $(LIBRARY) $(LINKER) -o sdl
