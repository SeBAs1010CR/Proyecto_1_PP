#ifndef HORARIO_H
#define HORARIO_H

#define MAX_DIA_LEN 10

typedef struct {
    char dia[MAX_DIA_LEN];
    int horaInicio;
    int horaFin;
} Horario;

#endif
