#pragma once

typedef unsigned long long u64;
typedef unsigned long      usize;

typedef struct {
    long fd;        // descriptor (>=0 si abierto)
} Emitter;

// Crea/trunca archivo con permisos 0644
int  em_open(Emitter *em, const char *path);

// Escribe exactamente len bytes
int  em_write(Emitter *em, const char *buf, usize len);

// Helpers sin stdio
int  em_puts(Emitter *em, const char *s);     // escribe hasta '\0'
int  em_putc(Emitter *em, char c);            // 1 byte
int  em_putu64(Emitter *em, u64 v);           // decimal

// Cierra archivo
int  em_close(Emitter *em);
