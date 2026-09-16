#ifndef CONFLICTOS_H
#define CONFLICTOS_H

#include "horario.h"
#include "grupo.h"
#include "curso.h"

/* Compara dos horarios individuales. Retorna 1 si chocan, 0 si no. */
int horariosChocan(Horario h1, Horario h2);

/* Compara todos los horarios de dos grupos. Retorna 1 si chocan, 0 si no. */
int gruposChocan(Grupo g1, Grupo g2);

/* Analiza todo el catálogo y marca los cursos que tienen choques en un arreglo paralelo */
void detectarChoques(Curso *cursos, int cantidadCursos, int *resultadosChoques);

#endif