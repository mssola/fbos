A RISC-V operating system devoted to running fizz/buzz processes.

FizzBuzz OS (or just `fbos`) is an excuse to better grasp the fundamentals of
low level RISC-V machines. In practice, this is a small Operating System kernel
that is able to launch three processes: one that simply prints "Fizz", another
that prints "Buzz", and a final third which prints "FizzBuzz". These processes
are unaware of each other and it's up to the kernel to schedule them by using
the timer interrupts as given by openSBI (`fizz` on % 3 seconds, `buzz` on % 5
seconds, and `fizzbuzz` on % 15 seconds).

This kernel provides just one system call, `write`, which allows any program to
pass the string to be written into the serial port.

## Build

In order to build `fbos` you only need GCC. That being said, if you are not on a
RISC-V system you will also need cross compilation tools for it (check your
distribution for this, on openSUSE I simply installed `cross-riscv64-gcc14`).
With that installed, simply set the `CROSS_COMPILE` environment variable as
you'd do for building the Linux kernel (e.g. in my case, using openSUSE, I set
it to `riscv64-suse-linux-`). After all of that, just:

```
$ make
```

And that's basically it! The build process should've produced a `fbos` binary
sitting at the root of the project.

### Verbose output

You will notice that the output is suspiciously quiet. You can go back into
verbose output by passing `V=1` to any make target. Hence, for the build step,
you can do something like:

```
$ make V=1
```

### Debug mode

By default the kernel will be built with optimizations on. You can disable this
by passing the `DEBUG=1` option to any make target. This will also come in handy
whenever you'd want to debug the kernel via QEMU+GDB. Read more on this below.

## Test
### QEMU

The easiest way to test the `fbos` binary from the build step is with QEMU. That
being said, this kernel only supports the bootflow of openSBI `fw_dynamic`. If
you have a recent enough QEMU, then that's not a problem and you can skip the
next step. Otherwise you might need to specify the BIOS firmware that you'll
have to compile from openSBI. If you are on that side, don't worry, it's not
that hard:

```
$ git clone https://github.com/riscv-software-src/opensbi.git
$ cd opensbi
$ make ARCH=riscv CROSS_COMPILE=<your-cross-compile> PLATFORM=generic
# => build/platform/generic/firmware/fw_dynamic.bin
```

With that simply set the `QEMU_BIOS` environment variable with the full path of
the resulting `fw_dynamic.bin` file.

Now make sure that you have the version that is able to emulate a RISC-V system.
After that, simply run:

```
$ make qemu
```

The `qemu` target can be paired with the `DEBUG` parameter that you can pass to
make. Hence, you can also call it like so:

```
$ make qemu DEBUG=1
```

This will make QEMU wait for a GDB connection. On another terminal then type:

```
$ make gdb
```

Now you have a debugging session for this kernel. If you want to skip ahead,
notice that the kernel starts at `PAGE_OFFSET` (see `include/fbos/mm.h`). Hence,
upon starting the GDB session you can simply type:

```
# 0x80200000 is the current value of PAGE_OFFSET.
(gdb) break *0x80200000
(gdb) continue
```

From there you are already out of firmware code and right into the kernel.

## Special thanks to

SUSE for organizing [Hack Week 24](https://hackweek.opensuse.org/24/projects).
This project was mainly developed during this time.
