#ifndef GRUPO_H
#define GRUPO_H

#include "horario.h"

#define MAX_HORARIOS_POR_GRUPO 10
#define MAX_TIPO_LEN 20
#define MAX_PROFESOR_LEN 100

typedef struct {
    int numero;
    char tipo[MAX_TIPO_LEN];
    char profesor[MAX_PROFESOR_LEN];
    Horario horarios[MAX_HORARIOS_POR_GRUPO];
    int cantidadHorarios;
} Grupo;

#endif
