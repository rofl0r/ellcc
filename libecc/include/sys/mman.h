#ifndef _SYS_MMAN_H_
#define _SYS_MMAN_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

// void *mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset);
// int munmap(void *start, size_t length);
int mprotect(const void *addr, size_t len, int prot);

#define PROT_READ   0x1     // Memory can be read.
#define PROT_WRITE  0x2     // Memory can be written.
#define PROT_EXEC   0x4     // Memory can be executed.
#define PROT_NONE   0x0     // Memory can not be accessed.

#ifdef __cplusplus
}
#endif
#endif
