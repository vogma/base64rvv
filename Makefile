SRC=src
BUILD=build
INCLUDE=-Iinclude -Ilibb64/include
LINK_LIB64=-Llibb64 -lb64
FLAGS=-march=rv64gcvzba -mabi=lp64d -O3 -static -Wall -g
CLANG_FLAGS=-O3 -march=rv64gcv -g -c

CC = riscv64-unknown-linux-gnu-gcc
AR = riscv64-unknown-linux-gnu-ar
CFLAGS = -Wall -O2
ARFLAGS = rcs

SOURCES = $(SRC)/base64_rvv_decode.c $(SRC)/base64_rvv_encode.c
OBJECTS = $(BUILD)/base64_rvv_decode.o $(BUILD)/base64_rvv_encode.o
LIBARY=libb64rvv.a

base64: Makefile
	$(RISCV)/riscv64-unknown-linux-gnu-gcc $(FLAGS) $(SRC)/$@.c $(SRC)/$@.S -o $(BUILD)/$@

base64_decode.o: Makefile
	clang $(CLANG_FLAGS) $(SRC)/base64_decode.c -o $(BUILD)/$@ $(INCLUDE)

base64_decode: base64_decode.o Makefile
	$(RISCV)/riscv64-unknown-linux-gnu-gcc $(FLAGS) $(BUILD)/$@.o -Llibb64 -lb64 -o $(BUILD)/$@

base64_decode_gcc: Makefile
	$(RISCV)/riscv64-unknown-linux-gnu-gcc $(FLAGS) $(SRC)/base64_decode.c -o $(BUILD)/base64_decode

cycleCount: Makefile
	$(RISCV)/riscv64-unknown-linux-gnu-gcc $(FLAGS) $(SRC)/$@.c $(SRC)/base64.S -o $(BUILD)/$@

base64_rvv_decode.o : lib/$(SRC)/base64_rvv_decode.c 
	$(CC) $(FLAGS) -Ilib/include -c $< -o lib/$(BUILD)/$@

base64_rvv_encode.o : lib/$(SRC)/base64_rvv_encode.c 
	$(CC) $(FLAGS) -Ilib/include -c $< -o lib/$(BUILD)/$@

$(LIBARY): base64_rvv_decode.o base64_rvv_encode.o
	$(AR) $(ARFLAGS) lib/$(BUILD)/$@  lib/$(BUILD)/base64_rvv_decode.o lib/$(BUILD)/base64_rvv_encode.o

test.o: $(SRC)/test.c
	$(CC) $(FLAGS) -Ilib/include -c $< -o $(BUILD)/$@

test: $(LIBARY) test.o 
	$(RISCV)/riscv64-unknown-linux-gnu-gcc $(FLAGS) $(BUILD)/test.o -o $(BUILD)/$@ -Llib/build -lb64rvv


clean: Makefile
	rm -f $(BUILD)/$(LIBARY)
	rm -f $(BUILD)/$(OBJECTS)
