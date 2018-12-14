PLUGIN := /home/connor/src/clang_randstruct/build/librandstruct.so

all:
	clang -Xclang -load -Xclang $(PLUGIN) -Xclang -add-plugin -Xclang swap-fields poc.c -o poc
