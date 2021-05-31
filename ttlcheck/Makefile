
PARAMS = -std=c99 -ladvapi32

# Targets

all:
	gcc main.c $(PARAMS) -o ttlcheck

run: all
	./ttlcheck

withicon:
	windres icon.rc -O coff -o icon.res.o
	gcc main.c $(PARAMS) icon.res.o -o ttlcheck
