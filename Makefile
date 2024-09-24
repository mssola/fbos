##
# By default everything is silent. If you want to change this behavior simply
# assign V=1 when calling make.

V =
ifeq ($(strip $(V)),)
	E = @echo
	Q = @
else
	E = @\#
	Q =
endif

##
# Compile options. You can use CROSS_COMPILE just like on the Linux Kernel. You
# can also determine the ISA to be used, which defaults to my VisionFive2 board.
# I did not go too much into the rabbit hole of platform-specific flags. Hence
# no `-mcpu`, no `-mtune`, no funny business.

CC    = $(CROSS_COMPILE)gcc$(CC_SUFFIX)
LD    = $(CROSS_COMPILE)ld
QEMU ?= qemu-system-riscv64

ISA     ?= rv64imafdc_zicntr_zicsr_zifencei_zihpm_zca_zcd_zba_zbb
CCFLAGS  = -march=$(ISA) -Iinclude/ -mabi=lp64d
CCFLAGS += -Werror -Wpedantic -Wall -Wextra -Wcast-align -Wcast-qual -Winit-self \
           -Wmissing-include-dirs -Wredundant-decls -Wshadow -Wsign-conversion \
           -Wswitch-default -Wundef -Wunreachable-code -Wmissing-noreturn
LDFLAGS  = -Iinclude/ -static -nostdlib -melf64lriscv -z noexecstack

##
# Optional parameters for QEMU and gdb.

QEMU_FLAGS      ?=
QEMU_BIOS       ?=
GDB_EXTRA_FLAGS ?=

##
# You can pass an optional `DEBUG` variable to manipulate the build type. This
# will also affect how QEMU is run.

DEBUG =
ifeq ($(strip $(DEBUG)),)
	CCFLAGS += -O3
else
	CCFLAGS += -g
	QEMU_FLAGS += -s -S
endif

##
# Paths.

SRC    = $(wildcard kernel/head.S kernel/*.c)
OBJ    = $(patsubst %.c,%.o,$(patsubst %.S,%.o,$(SRC)))
LINKER = kernel/fbos.ld
KRNL   = fbos

LDFLAGS += -T $(LINKER)

##
# Targets

.PHONY: all
all: clean $(KRNL)

.PHONY: $(KRNL)
$(KRNL): $(OBJ) $(LINKER).S
	$(Q) $(CC) -E $(LINKER).S -Iinclude/ -o $(LINKER)

	$(E) "	LD	" $@
	$(Q) $(LD) $(LDFLAGS) $(OBJ) -o $(KRNL)

.c.o:
	$(E) "	CC	" $(*F)
	$(Q) $(CC) $(CCFLAGS) -c $< -o $@

.S.o:
	$(E) "	CC	" $(*F)
	$(Q) $(CC) $(CCFLAGS) -c $< -o $@

.PHONY: qemu
qemu: $(KRNL)
ifeq ($(strip $(QEMU_BIOS)),)
	$(Q) $(QEMU) $(QEMU_FLAGS) -machine virt -kernel $(KRNL) -nographic
else
	$(Q) $(QEMU) $(QEMU_FLAGS) -machine virt -bios $(QEMU_BIOS) -kernel $(KRNL) -nographic
endif

.PHONY: gdb
gdb:
	$(Q) gdb --command utils/init.gdb $(GDB_EXTRA_FLAGS) $(KRNL)

.PHONY: clean
clean:
	$(Q) rm -f $(OBJ) $(KRNL) $(LINKER)

.PHONY: lint
lint:
	$(Q) git ls-files *.c *.h | xargs clang-format --dry-run --Werror
