/*
 * elegibilidad.c
 * 
 * Módulo: Validación de requisitos, correquisitos y elegibilidad
 * Autoría: Persona 3 - Bryan Sibaja
 * 
 * Propósito: Implementa las reglas que determinan si un curso puede
 * matricularse según el historial del estudiante.
 * 
 * Caso límite importante: los correquisitos mutuos (ej.: QU1102 y QU1106).
 * No se exige que el correquisito ya esté aprobado, porque eso haría
 * imposible matricular cualquiera de los dos. En su lugar se acepta que el
 * correquisito sea matriculable en el mismo semestre.
 */

#include <string.h>

#include "elegibilidad.h"

/*
 * buscarCurso
 * 
 * Busca un curso por su código dentro del arreglo de cursos.
 * 
 * Recibe: arreglo de cursos, cantidad, código a buscar.
 * Devuelve: puntero al curso encontrado, o NULL si no existe.
 */
static const Curso *buscarCurso(const Curso *cursos, int cantidad,
                                 const char *codigo) {
    for (int i = 0; i < cantidad; i++) {
        if (strcmp(cursos[i].codigo, codigo) == 0) return &cursos[i];
    }
    return NULL;
}

/* Verifica si todos los requisitos del curso están aprobados.
 * 
 * Regla: cada requisito debe estar en el historial de cursos aprobados.
 * 
 * Recibe: curso y historial.
 * Devuelve: 1 si cumple todos los requisitos, 0 si falta algún requisito.
 */
int cumpleRequisitos(const Curso *curso, const Historial *historial) {
    if (curso == NULL || historial == NULL) return 0;
    for (int i = 0; i < curso->requisitos.cantidad; i++) {
        if (!cursoAprobado(historial, curso->requisitos.codigos[i])) {
            return 0; /* Falta un requisito */
        }
    }
    return 1;
}

/* Verifica si se cumplen todos los correquisitos.
 * 
 * Para cada correquisito:
 * - Si ya está aprobado (en historial) → OK.
 * - Si NO está aprobado → debe ser matriculable en este semestre.
 *   Eso significa: existe en el catálogo y cumple sus propios requisitos.
 *   (permite correquisitos mutuos, ej.: A requiere B y B requiere A).
 * 
 * Recibe: curso, historial, catálogo y cantidad de cursos.
 * Devuelve: 1 si cumple todos los correquisitos, 0 si no.
 */
int cumpleCorrequisitos(const Curso *curso, const Historial *historial,
                         const Curso *cursos, int cantidadCursos) {
    if (curso == NULL || historial == NULL) return 0;

    for (int i = 0; i < curso->correquisitos.cantidad; i++) {
        const char *codigo = curso->correquisitos.codigos[i];
        /* Caso 1: ya está aprobado */
        if (cursoAprobado(historial, codigo)) {
            continue;
        }

        /* Caso 2: no está aprobado, verificar si es matriculable este semestre */
        const Curso *otro = buscarCurso(cursos, cantidadCursos, codigo);
        if (otro == NULL) {
            return 0; /* Correquisito no existe en catálogo */
        }
        if (!cumpleRequisitos(otro, historial)) {
            return 0; /* No cumple requisitos para matricularse este semestre */
        }
        /* Si cumple requisitos, se considera matriculable en mismo semestre → OK */
    }
    return 1;
}

/* Determina si cada curso puede matricularse.
 * 
 * Reglas aplicadas:
 * 1. Si el curso ya está aprobado en el historial → puedeMatricular = 0
 *    (no se puede volver a matricular).
 * 2. Si no está aprobado → puedeMatricular = (cumple todos los requisitos AND
 *    cumple todos los correquisitos).
 * 
 * Recibe: arreglo de cursos, cantidad y historial.
 * Devuelve: nada (modifica el campo puedeMatricular de cada curso).
 */
void determinarElegibilidad(Curso *cursos, int cantidadCursos,
                            const Historial *historial) {
    if (cursos == NULL || historial == NULL) return;

    for (int i = 0; i < cantidadCursos; i++) {
        Curso *curso = &cursos[i];

        /* Regla 3: Un curso ya aprobado no se vuelve a matricular */
        if (cursoAprobado(historial, curso->codigo)) {
            curso->puedeMatricular = 0;
            continue;
        }

        /* Evaluar requisitos y correquisitos */
        int cumpleReq = cumpleRequisitos(curso, historial);
        int cumpleCorreq = cumpleCorrequisitos(curso, historial, cursos, cantidadCursos);
        
        curso->puedeMatricular = cumpleReq && cumpleCorreq;
    }
}
