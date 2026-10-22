/*
 * conflictos.c
 * 
 * Módulo: Detección de choques de horario
 * Autoría: Persona 2 - Ian Alejandro Bonilla Mena
 * 
 * Propósito: Implementa la detección de superposición entre horarios, grupos y cursos.
 * 
 * Nota importante sobre el caso límite: cuando un horario termina exactamente
 * a la hora que otro inicia (ej.: 8:00-10:00 y 10:00-12:00), NO se considera
 * choque. Esto se logra con la comparación estricta < en ambas condiciones.
 */

#include <string.h>
#include "conflictos.h"

/* Verifica si dos horarios chocan entre sí.
 * 
 * Condiciones para que haya choque:
 * - Deben ser el mismo día (strcmp == 0).
 * - Sus intervalos deben superponerse: h1.horaInicio < h2.horaFin AND h2.horaInicio < h1.horaFin
 *   (con < estricto, evita choque cuando son contiguos: fin == inicio).
 * 
 * Recibe: dos Horarios h1 y h2.
 * Devuelve: 1 si chocan, 0 si no chocan.
 */
int horariosChocan(Horario h1, Horario h2) {
    /* Comparar días: solo pueden chocar si son el mismo día */
    if (strcmp(h1.dia, h2.dia) != 0) {
        return 0; /* Días distintos, no hay choque */
    }

    /* Verificar superposición de intervalos horarios.
     * Se usa < estricto para no reportar choque cuando terminan y empiezan a la misma hora */
    if ((h1.horaInicio < h2.horaFin) && (h2.horaInicio < h1.horaFin)) {
        return 1; /* Hay superposición → choque */
    }

    return 0; /* No hay superposición o son horarios contiguos */
}

/* Verifica si hay choque entre dos grupos.
 * 
 * Dos grupos chocan si al menos un horario de g1 choca con algún horario de g2.
 * Se usa salida temprana al encontrar el primer choque para optimizar.
 * 
 * Recibe: dos Grupos g1 y g2.
 * Devuelve: 1 si chocan, 0 si no.
 */
int gruposChocan(Grupo g1, Grupo g2) {
    /* Comparar todos los pares de horarios entre grupos */
    for (int i = 0; i < g1.cantidadHorarios; i++) {
        for (int j = 0; j < g2.cantidadHorarios; j++) {
            if (horariosChocan(g1.horarios[i], g2.horarios[j])) {
                return 1; /* Se encontró al menos un choque */
            }
        }
    }
    return 0; /* No hay choques entre estos grupos */
}

/* Analiza todo el catálogo para detectar choques entre cursos.
 * 
 * Estrategia:
 * - Inicializa todos los resultados a 0.
 * - Compara cada par de cursos (i,j) con j > i para evitar auto-comparación
 *   y comparaciones duplicadas (A vs B es lo mismo que B vs A).
 * - Para cada par, compara todos sus grupos entre sí. Con bandera hayChoque
 *   y condición && !hayChoque para salida temprana al encontrar primer choque.
 * - Si hay choque entre algún par de grupos, marca ambos cursos en resultadosChoques.
 * 
 * Resultados se almacenan en arreglo paralelo (no altera estructuras originales).
 * 
 * Recibe: cursos, cantidadCursos, resultadosChoques (arreglo de tamaño cantidadCursos).
 * Devuelve: nada.
 */
void detectarChoques(Curso *cursos, int cantidadCursos, int *resultadosChoques) {
    /* Inicializar arreglo de resultados: 0 = sin choque */
    for (int i = 0; i < cantidadCursos; i++) {
        resultadosChoques[i] = 0;
    }

    /* Comparar todos los pares de cursos */
    for (int i = 0; i < cantidadCursos; i++) {
        for (int j = i + 1; j < cantidadCursos; j++) {
            int hayChoque = 0;

            /* Comparar todos los grupos entre curso i y curso j */
            for (int g1 = 0; g1 < cursos[i].cantidadGrupos && !hayChoque; g1++) {
                for (int g2 = 0; g2 < cursos[j].cantidadGrupos && !hayChoque; g2++) {
                    if (gruposChocan(cursos[i].grupos[g1], cursos[j].grupos[g2])) {
                        hayChoque = 1; /* Se detectó choque entre estos cursos */
                    }
                }
            }

            /* Si hay choque, marcar ambos cursos */
            if (hayChoque) {
                resultadosChoques[i] = 1;
                resultadosChoques[j] = 1;
            }
        }
    }
}
