#ifndef ELEGIBILIDAD_H
#define ELEGIBILIDAD_H

/*
 * elegibilidad.h
 * 
 * Módulo: Validación de requisitos y correquisitos (Persona 3 - Bryan Sibaja)
 * Propósito: Determina si un curso puede matricularse según el historial
 * del estudiante y las reglas de requisitos/correquisitos.
 * 
 * Reglas:
 * 1. Un requisito se cumple solo si el curso aparece en el historial de aprobados.
 * 2. Un correquisito se cumple si ya está aprobado, o si el estudiante puede
 *    matricularlo en el mismo semestre (existe en catálogo y cumple sus requisitos).
 * 3. Un curso que ya está aprobado NO se puede volver a matricular (puedeMatricular=0).
 * 
 * Caso especial: correquisitos mutuos (ej. QU1102 y QU1106). No se puede exigir
 * que el correquisito ya esté aprobado, porque eso crearía un ciclo. Por eso
 * se acepta también que el correquisito sea "matriculable en el mismo semestre".
 */

#include "curso.h"
#include "historial.h"

/* Verifica si el curso cumple con todos sus requisitos.
 * 
 * Todos los códigos en la lista de requisitos deben estar en el historial
 * de cursos aprobados.
 * 
 * Recibe: curso y historial.
 * Devuelve: 1 si cumple todos los requisitos, 0 si no.
 */
int cumpleRequisitos(const Curso *curso, const Historial *historial);

/* Verifica si el curso cumple con todos sus correquisitos.
 * 
 * Para cada correquisito: o ya está aprobado (en historial), o es matriculable
 * en este semestre (existe en catálogo y cumple sus propios requisitos).
 * Esto permite manejar correquisitos mutuos.
 * 
 * Recibe: curso, historial, catálogo completo y cantidad de cursos.
 * Devuelve: 1 si cumple todos los correquisitos, 0 si no.
 */
int cumpleCorrequisitos(const Curso *curso, const Historial *historial,
                         const Curso *cursos, int cantidadCursos);

/* Determina la elegibilidad de todos los cursos del catálogo.
 * 
 * Para cada curso: si ya está aprobado → puedeMatricular = 0.
 * De lo contrario → puedeMatricular = (cumple requisitos AND cumple correquisitos).
 * 
 * Recibe: arreglo de cursos, cantidad, y historial.
 * Devuelve: nada (modifica curso->puedeMatricular).
 */
void determinarElegibilidad(Curso *cursos, int cantidadCursos,
                            const Historial *historial);

#endif
