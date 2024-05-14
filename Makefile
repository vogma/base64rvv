SRC=src
BUILD=build
FLAGS=-march=rv64gcvzba -mabi=lp64d -O3 -static -Wall -g -I/$(SRC)/base64.h
CLANG_FLAGS=-O3 -march=rv64gcv -g -c

base64: Makefile
	$(RISCV)/riscv64-unknown-linux-gnu-gcc $(FLAGS) $(SRC)/$@.c $(SRC)/$@.S -o $(BUILD)/$@

base64_decode.o: Makefile
	clang $(CLANG_FLAGS) $(SRC)/base64_decode.c -o $(BUILD)/$@ -I/$(SRC)/base64.h

base64_decode: base64_decode.o Makefile
	$(RISCV)/riscv64-unknown-linux-gnu-gcc $(FLAGS) $(BUILD)/$@.o $(SRC)/base64.h -o $(BUILD)/$@


cycleCount: Makefile
	$(RISCV)/riscv64-unknown-linux-gnu-gcc $(FLAGS) $(SRC)/$@.c $(SRC)/base64.S -o $(BUILD)/$@


clean: Makefile
	rm -r $(BUILD)/
