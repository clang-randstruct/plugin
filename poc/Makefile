PLUGIN := ../build/lib/librandstruct.so

RAND-SEED := 0

RAND-SEED-SCRIPT:=$(shell python ../rand-seed.py $(RAND-SEED))

all:
	clang -Xclang -load -Xclang $(PLUGIN) -Xclang -add-plugin -Xclang randstruct -Xclang -plugin-arg-randstruct -Xclang $(RAND-SEED-SCRIPT) poc.c -o poc
