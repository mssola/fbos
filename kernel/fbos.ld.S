#include <fbos/mm.h>

SECTIONS {
	// Ensure that the image starts at the very exact address SBI expects it to.
	. = LINK_ADDR;

	// You would usually want to separate the head section into its own thing
	// instead of clumping it into the main `.text` one. Well, I'm no expert on
	// linker configuration, so patches are welcome :)
	.text : {
		// The very first thing has to be the `_start` function, which is where
		// SBI will jump into. Afterwards comes the rest of `.text.head`.
		_start = .;
		*(.text.head)

		// Aligning it to a full page is maybe a bit too much considering how
		// small `.text.head` really is. I just saw this same thing on the Linux
		// Kernel and it felt clean.
		. = ALIGN(PAGE_SIZE);

		// From now on the rest of the `.text` could just be a continuation, but
		// I further split it into `.text.kernel` so the jump from head isn't
		// that large. That is, we put first the very core of the kernel, and
		// the rest can go wherever.
		__kernel_text_start = .;
		*(.text.kernel)
		__kernel_text_end = .;

		// And the rest.
    	*(.text)
	}

	// In total we would reserve two full blown pages for the kernel code, which
	// is definitely too much, but whatever, memory is cheap.
	. = ALIGN(PAGE_SIZE);

	.data : {
    	*(.data)
	}

	. = ALIGN(8);

	.rodata : {
    	*(.rodata)
	}

	. = ALIGN(8);

	.bss : {
    	*(.bss)
	}
}
