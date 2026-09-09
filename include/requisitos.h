#ifndef REQUISITOS_H
#define REQUISITOS_H

#define MAX_REQUISITOS 10
#define MAX_CODIGO_LEN 20

typedef struct {
    char codigos[MAX_REQUISITOS][MAX_CODIGO_LEN];
    int cantidad;
} Requisitos;

#endif
