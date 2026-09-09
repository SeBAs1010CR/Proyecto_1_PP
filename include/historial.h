#ifndef HISTORIAL_H
#define HISTORIAL_H

#include "requisitos.h"

#define MAX_CURSOS_APROBADOS 100

typedef struct {
    char codigos[MAX_CURSOS_APROBADOS][MAX_CODIGO_LEN];
    int cantidadCursos;
} Historial;

#endif
