#ifndef HISTORIAL_H
#define HISTORIAL_H

#include "requisitos.h"

#define MAX_CURSOS_APROBADOS 100

typedef struct {
    char codigos[MAX_CURSOS_APROBADOS][MAX_CODIGO_LEN];
    int cantidadCursos;
} Historial;

/* Carga data/historial.csv. Devuelve: la cantidad de cursos aprobados
 * cargados, o -1 si el puntero es nulo. */
int cargarHistorial(Historial *historial);

/* Devuelve: 1 si el curso está en el historial, 0 si no. */
int cursoAprobado(const Historial *historial, const char *codigo);

#endif
