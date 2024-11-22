#include <fbos/init.h>
#include <fbos/sbi.h>
#include <fbos/printk.h>

// TODO: this is QEMU-specific. To obtain this:
//   - Parse the DTB and look for the 'cpus.timebase-frequency' property.
//   - If the system is on ACPI (e.g. VisionFive2), then the frequency has to be
//     picked up from somewhere else (constant on known boards?).
#define TICKS_PER_SECOND 10000000

// Mask for 'scause' to check whether it came from an interrupt or an exception.
#define INTERRUPT_MASK 0x8000000000000000
#define IS_EXCEPTION(x) ((x & INTERRUPT_MASK) == 0)

// Mask for 'scause' to figure out if the interrupt was caused by the timer.
#define TIMER_SCAUSE_MASK 0x05

// Declared in include/fbos/init.h.
uint64_t seconds_elapsed;

// Set up a timer through the SBI interface that sends an interrupt in one
// second from the time this function is called.
__kernel void time_out_in_one_second(void)
{
	struct sbi_ret ret;
	register uint64_t one_second asm("a0");

	asm volatile("rdtime t0\n\t"
				 "li t1, %1\n\t"
				 "add %0, t0, t1"
				 : "=r"(one_second)
				 : "i"(TICKS_PER_SECOND)
				 : "t0", "t1");

	ret = sbi_ecall1(TIME_EXT, TIME_SET_TIMER, one_second);
	if (ret.error != SBI_SUCCESS) {
		die("Could not set timer\n");
	}
}

/*
 * Direct interrupt handler. Handles interrupts such as the timer event and user
 * mode entries.
 *
 * NOTE: as per RISC-V specification, the handler's address as set on the
 * 'stvec' register *must* be aligned on a 4-byte boundary. Hence, ensuring a
 * proper alignment is mandatory.
 *
 * NOTE: the '__s_interrupt' attribute already handles the saving/restoring of
 * all registers. It's probably a bit over the top since it also does that for
 * registers we never care on this kernel (e.g. floating point registers), but
 * it's convenient.
 */
__aligned(4) __s_interrupt __kernel void interrupt_handler(void)
{
	uint64_t cause;
	asm volatile("csrr %0, scause" : "=r"(cause)::);

	if (IS_EXCEPTION(cause)) {
		die("Don't know how to handle exceptions :D\n");
	}

	if ((cause & TIMER_SCAUSE_MASK) == TIMER_SCAUSE_MASK) {
		// Clear timer interrupt pending bit from the 'sip' register. Also clear
		// the timer interrupt enable so it's re-enabled after running the
		// fizz/buzz logic.
		asm volatile("li t0, 32\n\t"
					 "csrc sip, t0\n\t"
					 "csrc sie, t0"
					 :
					 :
					 : "t0");

		// BEHOLD! The fizz buzz logic! :D
		seconds_elapsed += 1;
		if ((seconds_elapsed % 15) == 0) {
			printk("Should run fizzbuzz\n");
		} else if ((seconds_elapsed % 5) == 0) {
			printk("Should run buzz\n");
		} else if ((seconds_elapsed % 3) == 0) {
			printk("Should run fizz\n");
		}

		// Re-enable timer interrupts.
		asm volatile("li t0, 32\n\t"
					 "csrs sie, t0"
					 :
					 :
					 : "t0");

		// Reset the timer one second from now.
		time_out_in_one_second();
	} else {
		printk("WARN: unknown interrupt just came in...\n");
	}
}

__kernel void setup_interrupts(void)
{
	/*
	 * - stvec:   point to our interrupt handler. The two least-significant bits are
	 *            going to be '00', meaning we are using direct mode.
	 * - sstatus: set the SIE (S Interrupt Enable) bit. Interrupts are now on!
	 */
	asm volatile("csrw stvec, %0\n\t"
				 "csrsi sstatus, 2"
				 :
				 : "r"(&interrupt_handler)
				 :);

	/*
	 * - sie: set bit 5 (STIE: S Timer Interrupt Enable).
	 *
	 * NOTE: head.S zeroes out both 'sip' and 'sie' registers. Hence, there are
	 * no pending interrupts.
	 */
	asm volatile("li t0, 32\n\t"
				 "csrs sie, t0"
				 :
				 :
				 : "t0");

	// And initialize the timer to send an interrupt in one second from now.
	time_out_in_one_second();
}
