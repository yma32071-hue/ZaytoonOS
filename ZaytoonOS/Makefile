CC = gcc
CFLAGS = -std=gnu11 -nostdlib -m32 -ffreestanding -O2 -Wall -Wextra -fno-common -fno-builtin -fno-pic -fno-pie -fno-stack-protector -mno-sse -mno-sse2 -mno-mmx -I.
LDFLAGS = -m32 -Wl,-m,elf_i386 -ffreestanding -nostdlib -no-pie -static -T linker.ld
BUILD_DIR = build
APPCOMPILER_DIR = appcompiler
GENERATED_CAPPS = kernel/apps/generated_capps.c
PACKAGED_CAPPS = $(APPCOMPILER_DIR)/commandline.capp
SRC_DIRS = . kernel kernel/arch kernel/irq kernel/mm kernel/drivers kernel/kernel kernel/user kernel/apps kernel/fs kernel/shell

SRC_C = $(sort $(wildcard $(addsuffix /*.c,$(SRC_DIRS))))
SRC_S = $(sort $(wildcard $(addsuffix /*.S,$(SRC_DIRS))))

OBJ = $(patsubst %.S,$(BUILD_DIR)/%.o,$(SRC_S)) $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC_C))

DEP = $(OBJ:.o=.d)
OUT = krnl.elf

all: $(GENERATED_CAPPS) $(OUT)

$(GENERATED_CAPPS): $(PACKAGED_CAPPS) tools/embed_capps.sh
	./tools/embed_capps.sh $@ $(PACKAGED_CAPPS)

$(PACKAGED_CAPPS): $(APPCOMPILER_DIR)/commandline.c $(APPCOMPILER_DIR)/lib.c $(APPCOMPILER_DIR)/lib.h
	$(MAKE) -C $(APPCOMPILER_DIR)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT): $(GENERATED_CAPPS) $(OBJ)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) $(BUILD_DIR)/kernel/arch/boot.o $(filter-out $(BUILD_DIR)/kernel/arch/boot.o, $(OBJ)) -o $@

run: $(OUT)
	qemu-system-x86_64 -machine accel=tcg -serial stdio -display none -kernel $(OUT)

clean:
	rm -rf $(BUILD_DIR) $(OUT)
	$(MAKE) -C $(APPCOMPILER_DIR) clean

.PHONY: all run clean

-include $(DEP)
