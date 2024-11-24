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

CC     = $(CROSS_COMPILE)gcc$(CC_SUFFIX)
LD     = $(CROSS_COMPILE)ld
HOSTCC = gcc$(CC_SUFFIX)
QEMU ?= qemu-system-riscv64

ISA      ?= rv64imafdc_zicntr_zicsr_zifencei_zihpm_zca_zcd_zba_zbb
ASFLAGS   = -march=$(ISA) -mabi=lp64d -mcmodel=medany -fno-PIE
CCFLAGS   = $(ASFLAGS) -Iinclude/ -D__KERNEL__ -std=gnu17 -nostdinc -nostdlib
WARNINGS  = -Werror -Wpedantic -Wall -Wextra -Wcast-align -Wcast-qual -Winit-self \
            -Wmissing-include-dirs -Wredundant-decls -Wshadow -Wsign-conversion \
            -Wswitch-default -Wundef -Wunreachable-code
CCFLAGS  += $(WARNINGS)
LDFLAGS   = -Iinclude/ -static -melf64lriscv -z noexecstack
USRFLAGS  = -static -melf64lriscv

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
	QEMU_FLAGS += -nographic
else
	ASFLAGS += -g
	CCFLAGS += -g
	QEMU_FLAGS += -s -S
endif

##
# Paths

SRC    = $(filter-out kernel/fbos.ld.S, $(wildcard kernel/*.S kernel/*.c lib/*.c))
OBJ    = $(patsubst %.c,%.o,$(patsubst %.S,%.o,$(SRC)))
LINKER = kernel/fbos.ld
KRNL   = fbos
USR    = usr/bin/init usr/bin/fizz usr/bin/buzz usr/bin/fizzbuzz
INIT   = usr/initramfs.cpio
TESTS  = test/test_dt test/test_initrd

LDFLAGS += -T $(LINKER)

##
# Kernel

.PHONY: all
all: clean $(KRNL) usr test

.PHONY: $(KRNL)
$(KRNL): $(OBJ) $(LINKER).S
	$(Q) $(CC) -E $(LINKER).S -Iinclude/ -o $(LINKER)

	$(E) "	LD	" $@
	$(Q) $(LD) $(LDFLAGS) $(OBJ) -o $(KRNL)

.c.o:
	$(E) "	CC	" $(basename $@)
	$(Q) $(CC) $(CCFLAGS) -c $< -o $@

.S.o:
	$(E) "	CC	" $(basename $@)
	$(Q) $(CC) $(CCFLAGS) -D__ASSEMBLY__ -c $< -o $@

##
# User space

.PHONY: usr
usr: $(USR)
	$(E) "	CPIO	" $(INIT)
	$(Q) find usr/bin/ -type f -executable | cpio -o --quiet -H newc > $(INIT)

usr/src/%.o: usr/src/%.S
	$(E) "	CC	" $(basename $@)
	$(Q) $(CC) $(ASFLAGS) -D__ASSEMBLY__ -c $< -o $@

usr/bin/%: usr/src/%.o
	$(Q) mkdir -p usr/bin/
	$(E) "	LD	" $@
	$(Q) $(LD) $(USRFLAGS) $< -o $@

# HACK: do not remove object files from usr/src/.
.SECONDARY:

##
# Tests

.PHONY: test
test: host_lib usr $(TESTS)
	$(Q) ./test/test_dt
	$(Q) ./test/test_initrd

host_lib:
	$(Q) mkdir -p test/lib
	$(Q) $(HOSTCC) $(WARNINGS) -Iinclude/ -g -c lib/dt.c -o test/lib/dt.o
	$(Q) $(HOSTCC) $(WARNINGS) -Iinclude/ -g -c kernel/initrd.c -o test/lib/initrd.o

test/%.o: test/%.c
	$(Q) $(HOSTCC) $(WARNINGS) -g -Iinclude/ -c $< -o $@

test/%: test/%.o
	$(E) "	TEST	" $@
	$(Q) $(HOSTCC) -Iinclude/ $< test/lib/*.o -o $@

##
# Hacking

.PHONY: qemu
qemu: clean $(KRNL) usr
ifeq ($(strip $(QEMU_BIOS)),)
	$(Q) $(QEMU) $(QEMU_FLAGS) -machine virt -kernel $(KRNL) -initrd $(INIT)
else
	$(Q) $(QEMU) $(QEMU_FLAGS) -machine virt -bios $(QEMU_BIOS) -kernel $(KRNL) -initrd $(INIT)
endif

.PHONY: gdb
gdb:
	$(Q) gdb --command utils/init.gdb $(GDB_EXTRA_FLAGS) $(KRNL)

.PHONY: clean
clean:
	$(Q) rm -f $(OBJ) $(KRNL) $(LINKER) $(USR) usr/src/*.o $(INIT) test/*.o test/lib/*.o $(TESTS)

.PHONY: lint
lint:
	$(Q) git ls-files *.c *.h | xargs clang-format --dry-run --Werror
