#include "sys_emitter.h"

// --- números de syscall Linux ---
#if defined(__x86_64__)
  #define SYS_OPENAT  257
  #define SYS_WRITE     1
  #define SYS_CLOSE     3
#elif defined(__aarch64__)
  #define SYS_OPENAT   56
  #define SYS_WRITE    64
  #define SYS_CLOSE    57
#else
  #error "Host no soportado (necesita Linux x86_64 o aarch64) para generar salida.s"
#endif

// --- flags/constantes ---
#define O_RDONLY   0
#define O_WRONLY   1
#define O_RDWR     2
#define O_CREAT    0100
#define O_TRUNC    01000
#define AT_FDCWD   -100
#define MODE_0644  0644

// --- syscalls crudas ---
static inline long sys_write(long fd, const void *buf, unsigned long len) {
#if defined(__x86_64__)
    long ret;
    register long rax asm("rax") = SYS_WRITE;
    register long rdi asm("rdi") = fd;
    register const void *rsi asm("rsi") = buf;
    register unsigned long rdx asm("rdx") = len;
    asm volatile("syscall"
        : "=a"(ret)
        : "a"(rax), "D"(rdi), "S"(rsi), "d"(rdx)
        : "rcx", "r11", "memory");
    return ret;
#elif defined(__aarch64__)
    register long x8 asm("x8") = SYS_WRITE;
    register long x0 asm("x0") = fd;
    register const void *x1 asm("x1") = buf;
    register unsigned long x2 asm("x2") = len;
    asm volatile("svc 0"
        : "+r"(x0)
        : "r"(x8), "r"(x1), "r"(x2)
        : "memory");
    return x0;
#endif
}

static inline long sys_close(long fd) {
#if defined(__x86_64__)
    long ret;
    register long rax asm("rax") = SYS_CLOSE;
    register long rdi asm("rdi") = fd;
    asm volatile("syscall"
        : "=a"(ret)
        : "a"(rax), "D"(rdi)
        : "rcx", "r11", "memory");
    return ret;
#elif defined(__aarch64__)
    register long x8 asm("x8") = SYS_CLOSE;
    register long x0 asm("x0") = fd;
    asm volatile("svc 0"
        : "+r"(x0)
        : "r"(x8)
        : "memory");
    return x0;
#endif
}

static inline long sys_openat(long dfd, const char *path, long flags, long mode) {
#if defined(__x86_64__)
    long ret;
    register long rax asm("rax") = SYS_OPENAT;
    register long rdi asm("rdi") = dfd;
    register const char *rsi asm("rsi") = path;
    register long rdx asm("rdx") = flags;
    register long r10 asm("r10") = mode;
    asm volatile("syscall"
        : "=a"(ret)
        : "a"(rax), "D"(rdi), "S"(rsi), "d"(rdx), "r"(r10)
        : "rcx", "r11", "memory");
    return ret;
#elif defined(__aarch64__)
    register long x8 asm("x8") = SYS_OPENAT;
    register long x0 asm("x0") = dfd;
    register const char *x1 asm("x1") = path;
    register long x2 asm("x2") = flags;
    register long x3 asm("x3") = mode;
    asm volatile("svc 0"
        : "+r"(x0)
        : "r"(x8), "r"(x1), "r"(x2), "r"(x3)
        : "memory");
    return x0;
#endif
}

// --- utilidades básicas ---
static usize cstr_len(const char *s) {
    usize n = 0; if (!s) return 0;
    while (s[n] != '\0') n++;
    return n;
}

static int write_all(long fd, const char *buf, usize len) {
    usize off = 0;
    while (off < len) {
        long r = sys_write(fd, buf + off, (unsigned long)(len - off));
        if (r < 0) return -1;
        off += (usize)r;
    }
    return 0;
}

// --- API pública ---
int em_open(Emitter *em, const char *path) {
    if (!em || !path) return -1;
    long fd = sys_openat(AT_FDCWD, path, O_WRONLY | O_CREAT | O_TRUNC, MODE_0644);
    if (fd < 0) return -1;
    em->fd = fd;
    return 0;
}

int em_write(Emitter *em, const char *buf, usize len) {
    if (!em || em->fd < 0 || !buf) return -1;
    return write_all(em->fd, buf, len);
}

int em_puts(Emitter *em, const char *s) {
    return em_write(em, s, cstr_len(s));
}

int em_putc(Emitter *em, char c) {
    return em_write(em, &c, 1);
}

int em_putu64(Emitter *em, u64 v) {
    char tmp[32];
    int i = 0;
    if (v == 0) return em_putc(em, '0');
    while (v) { tmp[i++] = (char)('0' + (v % 10)); v /= 10; }
    for (int j = i - 1; j >= 0; --j) if (em_putc(em, tmp[j]) != 0) return -1;
    return 0;
}

int em_close(Emitter *em) {
    if (!em || em->fd < 0) return -1;
    long r = sys_close(em->fd);
    em->fd = -1;
    return (r < 0) ? -1 : 0;
}
