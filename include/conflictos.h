#ifndef CONFLICTOS_H
#define CONFLICTOS_H

/*
 * conflictos.h
 * 
 * Módulo: Detección de choques de horario (Persona 2 - Ian Alejandro Bonilla Mena)
 * Propósito: Detecta si existen choques (superposición) de horarios entre
 * cursos y grupos del catálogo.
 * 
 * Algoritmo: compara intervalos horarios del mismo día. Caso límite importante:
 * cuando un curso termina exactamente a la misma hora en que inicia otro
 * (ej.: 8:00-10:00 y 10:00-12:00), NO se considera choque. Esto se logra con
 * la condición estricta: h1.horaInicio < h2.horaFin y h2.horaInicio < h1.horaFin.
 */

#include "horario.h"
#include "grupo.h"
#include "curso.h"

/* Compara dos horarios individuales para detectar choque.
 * 
 * Dos horarios chocan si son del mismo día y sus intervalos de tiempo
 * se superponen (no son contiguos). Horarios consecutivos (fin == inicio)
 * NO chocan.
 * 
 * Recibe: dos Horarios h1 y h2.
 * Devuelve: 1 si chocan, 0 si no chocan.
 */
int horariosChocan(Horario h1, Horario h2);

/* Compara todos los horarios entre dos grupos para detectar choque.
 * 
 * Un grupo choca con otro si existe al menos un par de horarios
 * (uno de cada grupo) que chocan entre sí.
 * 
 * Recibe: dos Grupos g1 y g2.
 * Devuelve: 1 si chocan, 0 si no chocan.
 */
int gruposChocan(Grupo g1, Grupo g2);

/* Analiza todo el catálogo y detecta choques entre cursos.
 * 
 * Compara todos los pares de cursos (evitando duplicados y auto-comparación).
 * Cuando detecta choque entre algún grupo de dos cursos, marca ambos cursos
 * con valor 1 en resultadosChoques. Usa salida temprana al encontrar primer
 * choque entre un par para optimizar rendimiento.
 * 
 * Importante: almacena resultados en arreglo paralelo (no modifica estructura Curso).
 * 
 * Recibe: arreglo de cursos, cantidad de cursos, arreglo paralelo resultadosChoques
 * (debe tener tamaño cantidadCursos). Cada posición i corresponde al curso i.
 */
void detectarChoques(Curso *cursos, int cantidadCursos, int *resultadosChoques);

#endif