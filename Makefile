SRC=src
BUILD=build
FLAGS=-march=rv64gcv -mabi=lp64d -O3 -static -Wall -g

base64: Makefile
	$(RISCV)/riscv64-unknown-linux-gnu-gcc $(FLAGS) $(SRC)/$@.c -o $(BUILD)/$@

cycleCount: Makefile
	$(RISCV)/riscv64-unknown-linux-gnu-gcc $(FLAGS) $(SRC)/$@.c $(SRC)/$@.S -o $(BUILD)/$@


clean: Makefile
	rm -r $(BUILD)/
