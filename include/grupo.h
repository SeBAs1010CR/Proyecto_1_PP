#ifndef GRUPO_H
#define GRUPO_H

#include "horario.h"
#include "constantes.h"

typedef struct {
    int numero;
    char tipo[MAX_TIPO_LEN];
    char profesor[MAX_PROFESOR_LEN];
    Horario horarios[MAX_HORARIOS_POR_GRUPO];
    int cantidadHorarios;
} Grupo;

/* Deja el grupo en un estado vacío. */
void inicializarGrupo(Grupo *grupo);

/* Agrega un horario al grupo. Devuelve: 1 agregado, 2 repetido, 0 máximo. */
int agregarHorario(Grupo *grupo, const Horario *horario);

/* Devuelve: 1 si los horarios son idénticos, 0 si no. */
int mismoHorario(const Horario *a, const Horario *b);

/* Devuelve: 1 si los grupos son idénticos, 0 si difieren. */
int gruposIguales(const Grupo *a, const Grupo *b);

/* Devuelve: 1 si el grupo es válido, 0 en caso contrario. */
int validarGrupo(const Grupo *grupo);

#endif
