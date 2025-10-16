#ifndef ERRORES_H
#define ERRORES_H

int  errores_init(const char* ruta);     // abre en modo "w" (limpia)
void errores_add(const char* linea);     // agrega una línea (con \n automático)
void errores_close(void);                // cierra

#endif
