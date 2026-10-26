#ifndef SALIDA_H
#define SALIDA_H

/*
 * salida.h
 * 
 * Módulo: Exportación de resultados (Persona 3 - Bryan Sibaja)
 * Propósito: Exporta el catálogo completo a un archivo CSV con formato
 * específico para la etapa 2 (Racket).
 * 
 * Formato de salida: output/catalogo_salida.csv
 * Separador: punto y coma (;)
 * Una línea por grupo de cada curso (no por curso).
 * 
 * Encabezado:
 * codigo;nombre;creditos;grupo;tipo;profesor;horarios;requisitos;correquisitos;choca;puede_matricular;carrera
 * 
 * Campos:
 * - horarios: bloques separados por '|' (ej.: "MIE 07:30-09:20|VIE 07:30-09:20")
 * - requisitos/correquisitos: códigos separados por coma (vacío si no hay)
 * - choca: 1 si el curso choca con al menos otro (tomado de arreglo paralelo)
 * - puede_matricular: 1 si cumple requisitos y correquisitos
 * - carrera: carrera que aporta el curso (la consume Racket, etapa 2)
 */

#include "curso.h"

/* Exporta el catálogo al archivo output/catalogo_salida.csv.
 * 
 * Escribe una línea por cada grupo de cada curso. El valor 'choca' es el mismo
 * para todos los grupos de un mismo curso (proviene del arreglo paralelo choques).
 * 
 * Recibe: arreglo de cursos, cantidad de cursos, arreglo paralelo choques
 * (puede ser NULL si no se ha calculado; en ese caso todos quedan con choca=0).
 * 
 * Devuelve: cantidad de líneas escritas, o -1 si hubo error al crear el archivo.
 */
int exportarCatalogo(const Curso *cursos, int cantidadCursos,
                     const int *choques);

#endif
