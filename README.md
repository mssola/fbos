A RISC-V operating system devoted to running fizz/buzz processes.

FizzBuzz OS (or just `fbos`) is an excuse to better grasp the fundamentals of
low level RISC-V machines. In practice, this is a small Operating System kernel
that is able to launch three processes: one that simply prints "Fizz", another
that prints "Buzz", and a final third which prints "FizzBuzz". These processes
are unaware of each other and it's up to the kernel to schedule them by using
the timer interrupts as given by openSBI (`fizz` on % 3 seconds, `buzz` on % 5
seconds, and `fizzbuzz` on % 15 seconds).

This kernel provides just one system call, `write`, which allows any program to
pass the string to be written into the serial port and wait for the kernel to
re-schedule it.

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

The easiest way to test the `fbos` binary from the build step is with QEMU. You
need a QEMU version that is recent enough (see [requirements](#requirements))
and that is able to virtualize a RISC-V system. With that, simply run:

```
$ make qemu
```

This will open up QEMU in `-nographic` mode (hence the serial output will be
simply redirected to stdout), and you will be able to see the whole thing
working. Just like this:

![Demo on QEMU](./doc/qemu.svg)

Moreover, the `qemu` target can be paired with the `DEBUG` parameter that you
can pass to make. Hence, you can also call it like so:

```
$ make qemu DEBUG=1
```

This will make QEMU wait for a GDB connection. On another terminal then type:

```
$ make gdb
```

Now you have a debugging session for this kernel with debug symbols loaded.
Hence, upon starting the GDB session you can simply type:

```
(gdb) break _start
(gdb) continue
```

From there you are already out of firmware code and right into the kernel.
Moreover, you can also pass the `GDB_EXTRA_FLAGS` variable to the `make gdb`
target. This way you can pass extra parameters to gdb, such as:

```
$ make gdb GDB_EXTRA_FLAGS="-tui"
```

And now you have started a GDB session with a nice TUI interface.

### VisionFive 2

This kernel can also be run on real hardware. In particular, I have tested it
with the Starfive VisionFive 2 board. I have tested this with an existing Linux
installation. In there, I have modified the bootloader configuration so I have
now this entry:

```
label l6
	menu label FizzBuzz OS
	linux /fbos
	initrd /initramfs.cpio
	fdtdir /dtbs/<versioned directory>
```

In order to get the needed files, you can use the `archive` make target:

```
make archive
```

Copy this tarball to your board and then place the `fbos` binary image and the
`initramfs.cpio` file into `/boot`. When you reset your board, you will get the
new entry from U-Boot and you will be able to run the kernel from there. Like
this:

![Demo on VisionFive 2](./doc/vf2.svg)

## Requirements

We do not want to support a myriad of different scenarios, but we want to keep
things simple. Hence, here there are some limitations/requirements:

- You need a recent enough OpenSBI running on your firmware. I have tested this
  on a QEMU which has OpenSBI v1.5, but any firmware that implements a Runtime
  SBI version of 2.0 should be fine.
- You are supposed to pass an `initrd` always. This kernel will not try to
  magically come up with a made up file system or try to fetch something from an
  existing one. An `initrd` is already provided for you on the default `make`
  target, and that's what you are supposed to be passing to the kernel.
- RISC-V is ambivalent on the endianness. Not this kernel. Out of simplicity we
  require a little-endian RISC-V system.
- This kernel makes use of the `Zaamo` Extension for Atomic Memory Operations,
  which is included under the `A` Extension for Atomic Instructions.

## Special thanks to

- SUSE for organizing [Hack Week 24](https://hackweek.opensuse.org/24/projects).
  This project was mainly developed during this time.
- [Ricardo B. Marliere](https://github.com/rbmarliere) for contributing to the
  project with code, ideas and shared knowledge.
- I have also taken lots of valuable input by reading [Popovic's
  blog](https://popovicu.com/), so thanks a lot for writing such clear articles
  on a rather obscure topic.
- In a similar way, I have also taken the time to read a lot of code from the
  Linux Kernel. My understanding of both RISC-V and the Linux Kernel itself has
  vastly improved with this exercise, so I'd also like to take the chance to be
  grateful to the many people who have contributed to this vast undertaking that
  is the Linux Kernel.
