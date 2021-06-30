LINKER = -lwinmm -std=c99
NOWARN = -Wno-return-type -Wno-parentheses -Wno-unused-variable
SOURCE = main2.c
OUTPUT = audio

all:
	gcc $(SOURCE) $(LINKER) -o $(OUTPUT) -DERR_MSG -Wall $(NOWARN)
	./$(OUTPUT)

d:
	gcc $(SOURCE) $(LINKER) -o $(OUTPUT) -O0 -g -DERR_MSG

r:
	gcc $(SOURCE) $(LINKER) -o $(OUTPUT) -O2 -s -DNDEBUG
