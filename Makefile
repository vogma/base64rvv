SRC=src
BUILD=build
INCLUDE=-Iinclude -Ilibb64/include -Ilib/include
LINK_LIB64=-Llibb64 -lb64
LINK_LIB_RVV=-Llib/build -lb64rvv
FLAGS=-march=rv64gcvzba -mabi=lp64d -O3 -static -Wall -g

VLEN=256
SIM=qemu-riscv64
QEMU_FLAGS=-cpu rv64,v=on,vext_spec=v1.0,vlen=$(VLEN),rvv_ta_all_1s=on,rvv_ma_all_1s=on,zfh=true,x-zvfh=true

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
	$(RISCV)/$(CC) $(FLAGS) $(BUILD)/base64.o -o $(BUILD)/$@ $(LINK_LIB_RVV)

simulation: base64
	$(SIM) $(QEMU_FLAGS) $(BUILD)/$<

clean: Makefile
	rm -f $(BUILD)/$(LIBARY)
	rm -f $(OBJECTS)
