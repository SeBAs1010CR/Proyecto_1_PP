#include <string.h>

#include "elegibilidad.h"

static const Curso *buscarCurso(const Curso *cursos, int cantidad,
                                const char *codigo) {
    for (int i = 0; i < cantidad; i++) {
        if (strcmp(cursos[i].codigo, codigo) == 0) return &cursos[i];
    }
    return NULL;
}

int cumpleRequisitos(const Curso *curso, const Historial *historial) {
    if (curso == NULL || historial == NULL) return 0;
    for (int i = 0; i < curso->requisitos.cantidad; i++) {
        if (!cursoAprobado(historial, curso->requisitos.codigos[i])) return 0;
    }
    return 1;
}

int cumpleCorrequisitos(const Curso *curso, const Historial *historial,
                        const Curso *cursos, int cantidadCursos) {
    if (curso == NULL || historial == NULL) return 0;

    for (int i = 0; i < curso->correquisitos.cantidad; i++) {
        const char *codigo = curso->correquisitos.codigos[i];
        if (cursoAprobado(historial, codigo)) continue;

        /* No está aprobado: solo sirve si se puede llevar este semestre. */
        const Curso *otro = buscarCurso(cursos, cantidadCursos, codigo);
        if (otro == NULL) return 0;
        if (!cumpleRequisitos(otro, historial)) return 0;
    }
    return 1;
}

void determinarElegibilidad(Curso *cursos, int cantidadCursos,
                            const Historial *historial) {
    if (cursos == NULL || historial == NULL) return;

    for (int i = 0; i < cantidadCursos; i++) {
        Curso *curso = &cursos[i];

        /* Un curso ya aprobado no se vuelve a matricular. */
        if (cursoAprobado(historial, curso->codigo)) {
            curso->puedeMatricular = 0;
            continue;
        }

        curso->puedeMatricular =
            cumpleRequisitos(curso, historial) &&
            cumpleCorrequisitos(curso, historial, cursos, cantidadCursos);
    }
}
