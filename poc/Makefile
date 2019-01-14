PLUGIN := ../build/lib/librandstruct.so

RAND-SEED := 0

RAND-SEED-SCRIPT:=$(shell python ../rand-seed.py $(RAND-SEED))

all:
	@echo MY_VAR IS "$(RAND-SEED-SCRIPT)"; clang -Xclang -load -Xclang $(PLUGIN) -Xclang -add-plugin -Xclang randstruct poc.c -o poc
