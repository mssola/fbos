# State of the project by the end of SUSE Hackweek 2024

As part of [SUSE Hack Week
24](https://hackweek.opensuse.org/24/projects/fizzbuzz-os) I started this
project with the scope in mind to at least have something running. This is what
I have achieved by the end of this week:

- The kernel knows how to parse the Device Tree blob to obtain the memory
  location of the `initrd` blob.
- The kernel can parse the `initrd` (as referenced by the Device Tree blob) to
  reference all the user-space binaries.
- The kernel can parse the ELF binary for each user-space binary in order to get
  their binary entry.
- The kernel can react to the OpenSBI timer and count seconds on QEMU.
- The kernel can schedule each binary entry depending on the requirement of the
  whole system.

Hence, by the end of the week I already got a kernel that could start on QEMU
and show the whole `fizz`, `buzz`, `fizzbuzz` strings as given by user-space
processes. Even with rough edges still to be polished, my initial goals have
been met.
