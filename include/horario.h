#ifndef HORARIO_H
#define HORARIO_H

#include "constantes.h"

typedef struct {
    char dia[MAX_DIA_LEN];
    int horaInicio;
    int horaFin;
} Horario;

/* Parsea un solo bloque de horario ("MIE[07:30-09:20]" o "Mar 09:30-11:20").
 * Devuelve: 1 si es válido, 0 en caso contrario. */
int parsearHorario(const char *bloque, Horario *horario);

/* Parsea un campo de horario completo ("MIE[07:30-09:20], VIE[07:30-09:20]").
 * Devuelve: cantidad de horarios (>=1), -1 si no caben, -2 si está mal. */
int parsearHorarios(const char *texto, Horario *horarios, int maxHorarios);

/* Devuelve: 1 si el horario es válido, 0 en caso contrario. */
int validarHorario(const Horario *horario);

#endif
