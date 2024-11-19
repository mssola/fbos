#ifndef __FBOS_SBI_H_
#define __FBOS_SBI_H_

// TODO
/* SBI_SUCCESS 0 Completed successfully */
/* SBI_ERR_FAILED -1 Failed */
/* SBI_ERR_NOT_SUPPORTED -2 Not supported */
/* SBI_ERR_INVALID_PARAM -3 Invalid parameter(s) */
/* SBI_ERR_DENIED -4 Denied or not allowed */
/* SBI_ERR_INVALID_ADDRESS -5 Invalid address(s) */
/* SBI_ERR_ALREADY_AVAILABLE -6 Already available */
/* SBI_ERR_ALREADY_STARTED -7 Already started */
/* SBI_ERR_ALREADY_STOPPED -8 Already stopped */
/* SBI_ERR_NO_SHMEM -9 Shared memory not available */

enum sbi_ext {
	DBCN_EXT = 0x4442434E,
};

enum dbcn_actions {
	DBCN_WRITE = 0x00,
};

struct sbi_ret {
	long error;
	long value;
};

extern struct sbi_ret __sbi_ecall(unsigned long arg0, unsigned long arg1, unsigned long arg2,
								  unsigned long arg3, unsigned long arg4, unsigned long arg5,
								  int fid, int ext);

#define sbi_ecall2(ext, fid, arg0, arg1) __sbi_ecall(arg0, arg1, 0, 0, 0, 0, fid, ext)

#endif // __FBOS_SBI_H_
