#include "errores.h"
#include <stdio.h>
#include <stdarg.h>

static FILE* g_err = NULL;

int errores_init(const char* ruta) {
    if (g_err) return 0;          
    g_err = fopen(ruta, "w");      
    return g_err ? 0 : -1;
}

void errores_add(const char* linea) {
    if (!g_err || !linea) return;
    fputs(linea, g_err);
    fputc('\n', g_err);            
    fflush(g_err);                 
}

void errores_close(void) {
    if (g_err) { fclose(g_err); g_err = NULL; }
}
