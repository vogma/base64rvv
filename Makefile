SRC=src
BUILD=build
INCLUDE=-Iinclude -Ilibb64/include -Ilib/include
LINK_LIB64=-Llibb64 -lb64
LINK_LIB_RVV=-Llib/build -lb64rvv
FLAGS=-march=rv64gcvzba -mabi=lp64d -O3 -static -Wall -g

CC = riscv64-unknown-linux-gnu-gcc
AR = riscv64-unknown-linux-gnu-ar
ARFLAGS = rcs

OBJECTS = $(BUILD)/base64_rvv_decode.o $(BUILD)/base64_rvv_encode.o
LIBARY=libb64rvv.a

base64_rvv_decode.o : lib/$(SRC)/base64_rvv_decode.c 
	$(CC) $(FLAGS) $(INCLUDE) -c $< -o lib/$(BUILD)/$@

base64_rvv_encode.o : lib/$(SRC)/base64_rvv_encode.c 
	$(CC) $(FLAGS) $(INCLUDE) -c $< -o lib/$(BUILD)/$@

$(LIBARY): base64_rvv_decode.o base64_rvv_encode.o
	$(AR) $(ARFLAGS) lib/$(BUILD)/$@  lib/$(BUILD)/base64_rvv_decode.o lib/$(BUILD)/base64_rvv_encode.o

base64.o: $(SRC)/base64.c
	$(CC) $(FLAGS) $(INCLUDE) -c $< -o $(BUILD)/$@

base64: $(LIBARY) base64.o
	$(RISCV)/riscv64-unknown-linux-gnu-gcc $(FLAGS) $(BUILD)/base64.o -o $(BUILD)/$@ $(LINK_LIB_RVV)

clean: Makefile
	rm -f $(BUILD)/$(LIBARY)
	rm -f $(OBJECTS)
