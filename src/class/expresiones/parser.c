#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "id.h"
#include "ejecutar.h"

TipoRetorno ejecutarParser(struct ASTNode *node, struct entorno *e)
{
    TipoRetorno res;
    res.valor = NULL;
    res.tipo = TIPO_NULO;
    res.control = CTRL_NORMAL;

    if (!node || !node->value || !node->left)
    {
        printf("Error: llamada de parser inválida\n");
        return res;
    }
    TipoRetorno val = ejecutar(node->left, e);

    if (strcmp(node->value, "parseInt") == 0)
    {
        if (val.tipo == TIPO_CADENA)
        {
            char *str = (char *)val.valor;
            int valido = 1;

            for (size_t i = 0; i < strlen(str); i++)
            {
                if (!isdigit((unsigned char)str[i]))
                {
                    valido = 0;
                    break;
                }
            }

            if (valido)
            {
                int *p = (int *)malloc(sizeof(int));
                *p = atoi(str);
                res.valor = p;
                res.tipo = TIPO_ENTERO;
            }
            else
            {
                printf("Error: parseInt espera una cadena de enteros validos. Recibido: '%s'\n", str);
                return res;
            }
        }
        else
        {
            printf("Error: parseInt espera una cadena.\n");
        }
    }
    else if (strcmp(node->value, "parseFloat") == 0)
    {
        if (val.tipo == TIPO_CADENA)
        {
            float *p = (float *)malloc(sizeof(float));
            *p = strtof((char *)val.valor, NULL);
            res.valor = p;
            res.tipo = TIPO_DECIMAL;
        }
        else
        {
            printf("Error: parseFloat espera una cadena.\n");
        }
    }
    else if (strcmp(node->value, "parseDouble") == 0)
    {
        if (val.tipo == TIPO_CADENA)
        {
            char *endptr;
            double *p = (double *)malloc(sizeof(double));
            *p = strtod((char *)val.valor, &endptr);

            if (*endptr != '\0')
            {

                printf("Error: parseDouble espera una cadena numerica valida. Recibido: '%s'\n", (char *)val.valor);
                free(p);
            }
            else
            {
                res.valor = p;
                res.tipo = TIPO_DOUBLE;
            }
        }
        else
        {
            printf("Error: parseDouble espera una cadena.\n");
        }
    }

    else if (strcmp(node->value, "valueOf") == 0)
    {
        if (val.tipo == TIPO_ENTERO)
        {
            int *entero = (int *)val.valor;
            char *str = (char *)malloc(12 * sizeof(char));
            sprintf(str, "%d", *entero);
            res.valor = str;
            res.tipo = TIPO_CADENA;
        }
        else if (val.tipo == TIPO_DECIMAL)
        {
            float *decimal = (float *)val.valor;
            char *str = (char *)malloc(20 * sizeof(char));
            sprintf(str, "%f", *decimal);
            res.valor = str;
            res.tipo = TIPO_CADENA;
        }
        else if (val.tipo == TIPO_DOUBLE)
        {
            double *doble = (double *)val.valor;
            char *str = (char *)malloc(32 * sizeof(char));
            sprintf(str, "%lf", *doble);
            res.valor = str;
            res.tipo = TIPO_CADENA;
        }
        else if (val.tipo == TIPO_CHAR)
        {
            char *caracter = (char *)val.valor;
            char *str = (char *)malloc(2 * sizeof(char));
            str[0] = *caracter;
            str[1] = '\0';
            res.valor = str;
            res.tipo = TIPO_CADENA;
        }
        else if (val.tipo == TIPO_BOOLEANO)
        {
            int *booleano = (int *)val.valor;
            char *str = (char *)malloc(6 * sizeof(char));
            sprintf(str, "%s", (*booleano) ? "true" : "false");
            res.valor = str;
            res.tipo = TIPO_CADENA;
        }
        else
        {
            printf("Error: valueOf espera un entero, decimal, char o booleano.\n");
        }
    }
    else
    {
        printf("Error: función de parser desconocida '%s'.\n", node->value);
    }

    return res;
}
