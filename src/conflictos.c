// Incluye la librería estándar de C para manejar cadenas de texto (strcmp)
#include <string.h>
// Incluye el archivo de cabecera con las definiciones de tipos (Horario, Grupo, Curso) y prototipos de funciones
#include "conflictos.h"

/* Devuelve 1 si los horarios chocan, 0 si no chocan */
int horariosChocan(Horario h1, Horario h2) {
    // Compara los campos "dia" de ambos horarios usando strcmp
    // strcmp retorna 0 cuando las cadenas son iguales,distinto de 0 si son diferentes
    if (strcmp(h1.dia, h2.dia) != 0) {
        // Si los días son distintos, no puede haber choque, retorna 0 (falso)
        return 0;
    }

    // Verifica si los intervalos de horas se superponen en el mismo día
    // La condición verifica: horaInicio de h1 < horaFin de h2 Y horaInicio de h2 < horaFin de h1
    if ((h1.horaInicio < h2.horaFin) && (h2.horaInicio < h1.horaFin)) {
        // Si ambas condiciones se cumplen, hay superposición de horarios, retorna 1 (verdadero)
        return 1;
    }

    // Si no hay superposición (los horarios son consecutivos o están separados), retorna 0
    return 0;
}

/* Devuelve 1 si hay conflicto entre cualquier horario de los dos grupos, 0 si no */
int gruposChocan(Grupo g1, Grupo g2) {
    // Recorre todos los horarios del primer grupo (g1)
    for (int i = 0; i < g1.cantidadHorarios; i++) {
        // Para cada horario de g1, recorre todos los horarios del segundo grupo (g2)
        for (int j = 0; j < g2.cantidadHorarios; j++) {
            // Llama a horariosChocan para comparar el horario i de g1 con el horario j de g2
            if (horariosChocan(g1.horarios[i], g2.horarios[j])) {
                // Si encuentra un choque, retorna 1 inmediatamente (no necesita seguir buscando)
                return 1;
            }
        }
    }
    // Si recorrió todos los pares de horarios y no encontró ningún choque, retorna 0
    return 0;
}

/* Analiza el catálogo y marca los choques en el arreglo paralelo */
void detectarChoques(Curso *cursos, int cantidadCursos, int *resultadosChoques) {
    // Inicializa todo el arreglo de resultados en 0 (asume que no hay choques al inicio)
    for (int i = 0; i < cantidadCursos; i++) {
        resultadosChoques[i] = 0;
    }

    // Recorre todos los cursos usando un bucle doble para comparar cada par de cursos
    for (int i = 0; i < cantidadCursos; i++) {
        // j empieza en i+1 para evitar comparar un curso consigo mismo y no repetir comparaciones (A vs B = B vs A)
        for (int j = i + 1; j < cantidadCursos; j++) {
            // Variable bandera que indica si se encontró un choque entre los cursos i y j
            int hayChoque = 0;

            // Recorre todos los grupos del curso i
            for (int g1 = 0; g1 < cursos[i].cantidadGrupos && !hayChoque; g1++) {
                // Recorre todos los grupos del curso j, se detiene si ya se encontró un choque (!hayChoque)
                for (int g2 = 0; g2 < cursos[j].cantidadGrupos && !hayChoque; g2++) {
                    // Compara el grupo g1 del curso i con el grupo g2 del curso j
                    if (gruposChocan(cursos[i].grupos[g1], cursos[j].grupos[g2])) {
                        // Si hay choque entre esos grupos, marca la bandera como verdadera
                        hayChoque = 1;
                    }
                }
            }

            // Si se encontró al menos un choque entre los cursos i y j
            if (hayChoque) {
                // Marca el curso i como tener choque en el arreglo de resultados
                resultadosChoques[i] = 1;
                // Marca el curso j como tener choque en el arreglo de resultados
                resultadosChoques[j] = 1;
            }
        }
    }
}
