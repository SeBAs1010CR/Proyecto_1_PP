#ifndef CURSO_H
#define CURSO_H

#include "grupo.h"
#include "requisitos.h"
#include "constantes.h"

typedef struct {
    char codigo[MAX_CODIGO_LEN];
    char nombre[MAX_NOMBRE_LEN];
    int creditos;
    int horas;
    Requisitos requisitos;
    Requisitos correquisitos;
    Grupo grupos[MAX_GRUPOS_POR_CURSO];
    int cantidadGrupos;
    int puedeMatricular; /* 1 si el estudiante puede matricularlo */
} Curso;

/* Configura un curso vacío. */
void inicializarCurso(Curso *curso);

/* Parsea una línea del CSV en un Curso. Devuelve: 0 ok, o un código
 * negativo indicando el tipo de error. */
int leerCurso(const char *linea, Curso *curso);

/* Carga todos los cursos desde data/cursos.csv. Devuelve: la cantidad de
 * cursos cargados, o -1 si no se pudo abrir el archivo. */
int cargarCursos(Curso *cursos, int maxCursos);

#endif
