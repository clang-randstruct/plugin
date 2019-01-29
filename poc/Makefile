PLUGIN := ../build/lib/librandstruct.so

RAND-SEED := 0

RAND-SEED-SCRIPT:=$(shell python ../rand-seed.py $(RAND-SEED))

all:
	clang -Xclang -load -Xclang $(PLUGIN) -Xclang -add-plugin -Xclang randstruct -Xclang -plugin-arg-randstruct -Xclang $(RAND-SEED-SCRIPT) -g poc.c -o poc
	objdump -SD poc > poc.asm

tests:


	clang -S -Xclang -load -Xclang $(PLUGIN) -Xclang -add-plugin -Xclang randstruct -Xclang -plugin-arg-randstruct -Xclang $(RAND-SEED-SCRIPT) -S -emit-llvm -o ../tests/with_plugin.ll poc.c -###

	clang -S -emit-llvm -o ../tests/without_plugin.ll poc.c -###
	python3 ../tests/diff_testing.py ../tests/without_plugin.ll ../tests/with_plugin.ll

diag:

	clang -cc1 -triple "x86_64-pc-linux-gnu" "-emit-llvm" "-disable-free" "-disable-llvm-verifier" "-main-file-name" "poc.c" "-mrelocation-model" "static" "-mthread-model" "posix" "-mdisable-fp-elim" "-fmath-errno" "-mconstructor-aliases" "-munwind-tables" "-fuse-init-array" "-target-cpu" "x86-64" "-dwarf-column-info" "-debugger-tuning=gdb" "-coverage-notes-file" "/clang-randstruct/poc/../tests/with_plugin.gcno" "-resource-dir" "/usr/lib/llvm-7/lib/clang/7.0.1" "-internal-isystem" "/usr/local/include" "-internal-isystem" "/usr/lib/llvm-7/lib/clang/7.0.1/include" "-internal-externc-isystem" "/usr/include/x86_64-linux-gnu" "-internal-externc-isystem" "/include" "-internal-externc-isystem" "/usr/include" "-fdebug-compilation-dir" "/clang-randstruct/poc" "-ferror-limit" "19" "-fmessage-length" "117" "-fobjc-runtime=gcc" "-fdiagnostics-show-option" "-fcolor-diagnostics" "-load" "../build/lib/librandstruct.so" "-add-plugin" "randstruct" "-plugin-arg-randstruct" "cfcd208495d565ef66e7dff9f98764da" "-o" "../tests/with_plugin.ll" "-x" "c" "poc.c" "-faddrsig"

	clang -cc1 "-triple" "x86_64-pc-linux-gnu" "-emit-llvm" "-disable-free" "-disable-llvm-verifier" "-main-file-name" "poc.c" "-mrelocation-model" "static" "-mthread-model" "posix" "-mdisable-fp-elim" "-fmath-errno" "-mconstructor-aliases" "-munwind-tables" "-fuse-init-array" "-target-cpu" "x86-64" "-dwarf-column-info" "-debugger-tuning=gdb" "-coverage-notes-file" "/clang-randstruct/poc/../tests/without_plugin.gcno" "-resource-dir" "/usr/lib/llvm-7/lib/clang/7.0.1" "-internal-isystem" "/usr/local/include" "-internal-isystem" "/usr/lib/llvm-7/lib/clang/7.0.1/include" "-internal-externc-isystem" "/usr/include/x86_64-linux-gnu" "-internal-externc-isystem" "/include" "-internal-externc-isystem" "/usr/include" "-fdebug-compilation-dir" "/clang-randstruct/poc" "-ferror-limit" "19" "-fmessage-length" "117" "-fobjc-runtime=gcc" "-fdiagnostics-show-option" "-fcolor-diagnostics" "-o" "../tests/without_plugin.ll" "-x" "c" "poc.c" "-faddrsig"

	python3 ../tests/diff_testing.py ../tests/without_plugin.ll ../tests/with_plugin.ll
