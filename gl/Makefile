LINKER = -lglu32 -lgdi32 -lopengl32 -std=c99

all:
	gcc main.c $(LINKER) -o gl
	./gl

debug:
	gcc main.c $(LINKER) -o gl -O0 -g
	
release:
	gcc main.c $(LINKER) -o gl -O2 -s -DNDEBUG
	