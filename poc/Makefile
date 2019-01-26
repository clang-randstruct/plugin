PLUGIN := ../build/lib/librandstruct.so

RAND-SEED := 0

RAND-SEED-SCRIPT:=$(shell python ../rand-seed.py $(RAND-SEED))

all:
	clang -Xclang -load -Xclang $(PLUGIN) -Xclang -add-plugin -Xclang randstruct -Xclang -plugin-arg-randstruct -Xclang $(RAND-SEED-SCRIPT) -g poc.c -o poc
	objdump -SD poc > poc.asm

tests:
	clang -Xclang -load -Xclang $(PLUGIN) -Xclang -add-plugin -Xclang randstruct -Xclang -plugin-arg-randstruct -Xclang $(RAND-SEED-SCRIPT) -S -emit-llvm -o ../tests/with_plugin.ll poc.c

	clang -S -emit-llvm -o ../tests/without_plugin.ll poc.c
	python3 ../tests/diff_testing.py ../tests/without_plugin.ll ../tests/with_plugin.ll
