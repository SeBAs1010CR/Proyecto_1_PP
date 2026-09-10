#ifndef REQUISITOS_H
#define REQUISITOS_H

#include "constantes.h"

typedef struct {
    char codigos[MAX_REQUISITOS][MAX_CODIGO_LEN];
    int cantidad;
} Requisitos;

/* Deja la lista de requisitos vacía. */
void inicializarRequisitos(Requisitos *req);

/* Devuelve: 1 si el código ya existe en la lista, 0 si no. */
int contieneCodigo(const Requisitos *req, const char *codigo);

/* Devuelve: 1 si las listas tienen los mismos códigos, 0 si no. */
int requisitosIguales(const Requisitos *a, const Requisitos *b);

/* Agrega un código. Devuelve: 0 ok, 1 vacío, 2 muy largo, 3 duplicado,
 * 4 máximo alcanzado. */
int agregarCodigo(Requisitos *req, const char *codigo);

#endif
