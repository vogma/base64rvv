SRC=src
BUILD=build
INCLUDE=include
FLAGS=-march=rv64gcvzba -mabi=lp64d -O3 -static -Wall -g -I$(INCLUDE)
CLANG_FLAGS=-O3 -march=rv64gcv -g -c

base64: Makefile
	$(RISCV)/riscv64-unknown-linux-gnu-gcc $(FLAGS) $(SRC)/$@.c $(SRC)/$@.S -o $(BUILD)/$@

base64_decode.o: Makefile
	clang $(CLANG_FLAGS) $(SRC)/base64_decode.c -o $(BUILD)/$@ -I$(INCLUDE) -Ilibb64/include

base64_decode: base64_decode.o Makefile
	$(RISCV)/riscv64-unknown-linux-gnu-gcc $(FLAGS) $(BUILD)/$@.o -Llibb64 -lb64 -o $(BUILD)/$@

base64_decode_gcc: Makefile
	$(RISCV)/riscv64-unknown-linux-gnu-gcc $(FLAGS) $(SRC)/base64_decode.c -o $(BUILD)/base64_decode

cycleCount: Makefile
	$(RISCV)/riscv64-unknown-linux-gnu-gcc $(FLAGS) $(SRC)/$@.c $(SRC)/base64.S -o $(BUILD)/$@


clean: Makefile
	rm -r $(BUILD)/
