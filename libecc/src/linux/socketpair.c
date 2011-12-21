#include <syscall.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/socketcall.h>

int socketpair(int domain, int type, int protocol, int sv[2])
{
#if defined(__NR_socketpair)
    return INLINE_SYSCALL(socketpair, 4, domain, type, protocol, (intptr_t)sv);
#elif defined(__i386__) | defined(__ppc64__) | defined(__ppc__)
#if __BIG_ENDIAN__
#define ENDIAN be
#elif __LITTLE_ENDIAN__
#define ENDIAN le
#else
#error ENDIAN is undefined
#endif
    struct linux_sys_socketpair_args args;
    args.domain.ENDIAN.datum = domain;
    args.type.ENDIAN.datum = type;
    args.protocol.ENDIAN.datum = protocol;
    args.rsv.ENDIAN.datum = sv;
    return INLINE_SYSCALL(socketcall, 2, LINUX_SYS_socketpair, &args);
#else
#error socketpair is undefined
#endif
}
