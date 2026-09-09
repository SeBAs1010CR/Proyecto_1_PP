#ifndef CURSO_H
#define CURSO_H

#include "grupo.h"
#include "requisitos.h"

#define MAX_GRUPOS_POR_CURSO 10
#define MAX_NOMBRE_LEN 100

typedef struct {
    char codigo[MAX_CODIGO_LEN];
    char nombre[MAX_NOMBRE_LEN];
    int creditos;
    int horas;
    Requisitos requisitos;
    Requisitos correquisitos;
    Grupo grupos[MAX_GRUPOS_POR_CURSO];
    int cantidadGrupos;
} Curso;

#endif
