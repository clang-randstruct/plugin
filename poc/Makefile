PLUGIN := ../build/librandstruct.so

all:
	clang -Xclang -load -Xclang $(PLUGIN) -Xclang -add-plugin -Xclang randstruct poc.c -o poc
