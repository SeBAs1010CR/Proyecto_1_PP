#ifndef HISTORIAL_H
#define HISTORIAL_H

/*
 * historial.h
 * 
 * Módulo: Historial
 * Propósito: Define la estructura Historial y funciones para cargar y consultar
 * el historial de cursos aprobados del estudiante.
 * 
 * El historial se lee desde data/historial.csv (un código de curso por línea).
 */

#include "requisitos.h"

#define MAX_CURSOS_APROBADOS 100 /* Máximo de cursos aprobados a almacenar */

/* Estructura que almacena los cursos aprobados por el estudiante.
 * 
 * - codigos: Lista de códigos de cursos aprobados (ej.: "CE1101", "MA1102").
 * - cantidadCursos: Cantidad de cursos aprobados cargados.
 */
typedef struct {
    char codigos[MAX_CURSOS_APROBADOS][MAX_CODIGO_LEN];
    int cantidadCursos;
} Historial;

/* Carga el historial desde data/historial.csv.
 * 
 * Si el archivo no existe, deja el historial vacío y muestra un aviso.
 * Ignora líneas vacías, códigos duplicados o códigos demasiado largos.
 * 
 * Recibe: puntero a Historial donde cargar.
 * Devuelve: cantidad de cursos aprobados cargados (0 si vacío o no existe),
 * o -1 si el puntero es nulo.
 */
int cargarHistorial(Historial *historial);

/* Verifica si un curso está aprobado en el historial.
 * 
 * Recibe: historial y código del curso a buscar.
 * Devuelve: 1 si el curso está aprobado, 0 si no lo está.
 */
int cursoAprobado(const Historial *historial, const char *codigo);

#endif
