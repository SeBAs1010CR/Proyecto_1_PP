#ifndef SALIDA_H
#define SALIDA_H

#include "curso.h"

/* Exporta el catálogo a output/catalogo_salida.csv (una línea por grupo).
 * `choques` es el arreglo paralelo que llena detectarChoques (puede ser
 * NULL si aún no se calculó). Devuelve: la cantidad de líneas escritas,
 * o -1 si hubo error. */
int exportarCatalogo(const Curso *cursos, int cantidadCursos,
                     const int *choques);

#endif
