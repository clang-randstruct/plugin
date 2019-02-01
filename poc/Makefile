PLUGIN := ../build/lib/librandstruct.so

RAND-SEED := 0

RAND-SEED-SCRIPT:=$(shell python ../rand-seed.py $(RAND-SEED))

all:
	clang -Xclang -load -Xclang $(PLUGIN) -Xclang -add-plugin -Xclang randstruct -Xclang -plugin-arg-randstruct -Xclang "-rand-seed" -Xclang -plugin-arg-randstruct -Xclang $(RAND-SEED-SCRIPT) -g poc.c -o poc
	objdump -SD poc > poc.asm
