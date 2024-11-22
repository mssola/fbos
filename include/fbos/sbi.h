#ifndef __FBOS_SBI_H_
#define __FBOS_SBI_H_

// Possible values for `sbi_ret.error` as defined in the SBI Specification 2.0.
enum sbi_ret_error {
	SBI_SUCCESS = 0, // Completed successfully
	SBI_ERR_FAILED = -1, // Failed
	SBI_ERR_NOT_SUPPORTED = -2, // Not supported
	SBI_ERR_INVALID_PARAM = -3, // Invalid parameter(s)
	SBI_ERR_DENIED = -4, // Denied or not allowed
	SBI_ERR_INVALID_ADDRESS = -5, // Invalid address(s)
	SBI_ERR_ALREADY_AVAILABLE = -6, // Already available
	SBI_ERR_ALREADY_STARTED = -7, // Already started
	SBI_ERR_ALREADY_STOPPED = -8, // Already stopped
	SBI_ERR_NO_SHMEM = -9, // Shared memory not available
};

// SBI extensions supported by this kernel.
enum sbi_ext {
	DBCN_EXT = 0x4442434E,
	TIME_EXT = 0x54494D45,
};

// Function IDs for the Debug Console Extension "DBCN".
enum dbcn_actions {
	DBCN_WRITE = 0x00,
};

// Function IDs for the Timer Extension "TIME".
enum time_actions {
	TIME_SET_TIMER = 0x00,
};

// Return value for any SBI ecall.
struct sbi_ret {
	long error;
	long value;
};

// Perform an SBI ecall. Never call this directly, use any of the macros as
// defined below.
extern struct sbi_ret __sbi_ecall(unsigned long arg0, unsigned long arg1, unsigned long arg2,
								  unsigned long arg3, unsigned long arg4, unsigned long arg5,
								  int fid, int ext);

#define sbi_ecall1(ext, fid, arg0) __sbi_ecall(arg0, 0, 0, 0, 0, 0, fid, ext)
#define sbi_ecall2(ext, fid, arg0, arg1) __sbi_ecall(arg0, arg1, 0, 0, 0, 0, fid, ext)

#endif // __FBOS_SBI_H_
