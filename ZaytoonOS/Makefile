CC = gcc
CFLAGS = -std=gnu11 -nostdlib -ffreestanding -O2 -Wall -Wextra -fno-common -fno-builtin -fno-builtin-memcpy -fno-builtin-memset -I.
LDFLAGS = -nostdlib -T linker.ld
BUILD_DIR = build
SRC_DIRS = . kernel kernel/arch kernel/irq kernel/mm kernel/drivers kernel/kernel

SRC_C = $(sort $(wildcard $(addsuffix /*.c,$(SRC_DIRS))))
SRC_S = $(sort $(wildcard $(addsuffix /*.S,$(SRC_DIRS))))

OBJ = $(patsubst %.S,$(BUILD_DIR)/%.o,$(SRC_S)) $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC_C))

DEP = $(OBJ:.o=.d)
OUT = zaytoonos.elf

all: $(OUT)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) $(BUILD_DIR)/kernel/arch/boot.o $(filter-out $(BUILD_DIR)/kernel/arch/boot.o, $(OBJ)) -o $@
	@cp $(OUT) /workspaces/ZaytoonOS/ZaytoonOS/$(OUT) 2>/dev/null || true

run: $(OUT)
	qemu-system-x86_64 -machine accel=tcg -serial stdio -display none -kernel $(OUT)

clean:
	rm -rf $(BUILD_DIR) $(OUT)

.PHONY: all run clean

-include $(DEP)