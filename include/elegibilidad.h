#ifndef ELEGIBILIDAD_H
#define ELEGIBILIDAD_H

#include "curso.h"
#include "historial.h"

/* Devuelve: 1 si todos los requisitos del curso están aprobados, 0 si no. */
int cumpleRequisitos(const Curso *curso, const Historial *historial);

/* Devuelve: 1 si cada correquisito está aprobado o se puede matricular
 * en el mismo semestre, 0 si no. */
int cumpleCorrequisitos(const Curso *curso, const Historial *historial,
                        const Curso *cursos, int cantidadCursos);

/* Calcula puedeMatricular para cada curso del catálogo. */
void determinarElegibilidad(Curso *cursos, int cantidadCursos,
                            const Historial *historial);

#endif
