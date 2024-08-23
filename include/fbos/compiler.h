#ifndef __FBOS_COMPILER_H
#define __FBOS_COMPILER_H

#define __noreturn __attribute__((__noreturn__))

#define __section(s) __attribute__((__section__(s)))
#define __kernel __section(".text.kernel")

#endif /* __FBOS_COMPILER_H */
