/*
 * grupo.c
 *
 * Funciones para crear, completar y validar un Grupo.
 * Un grupo se compone de: número, tipo, profesor y uno o varios Horario.
 */

#include <stdio.h>
#include <string.h>

#include "grupo.h"

/* Límite razonable para el número de grupo, consistente con el límite que
 * usa el cargador (curso.c). Un grupo no puede tener número mayor. */
#define MAX_NUMERO_GRUPO 9999

/*
 * Indica si el tipo de grupo es uno de los tipos que existen realmente en
 * data/cursos.csv. No se inventan tipos nuevos.
 */
static int tipoGrupoValido(const char *tipo) {
    if (tipo == NULL) return 0;

    static const char *tiposValidos[] = {
        "Regular", "Semipresencial", "Virtual", "Asistida", "Grupo RN"};
    size_t cant = sizeof(tiposValidos) / sizeof(tiposValidos[0]);

    for (size_t i = 0; i < cant; i++) {
        if (strcmp(tipo, tiposValidos[i]) == 0) return 1;
    }
    return 0;
}

/*
 * Indica si el nombre de un profesor es aceptable: no vacío, no compuesto
 * únicamente por espacios, de largo razonable y sin caracteres de control.
 *
 * Se permiten tildes, ñ y otros caracteres UTF-8 (todos sus bytes son
 * mayores que 0x7F), así que los nombres válidos no se alteran.
 */
static int profesorValido(const char *profesor) {
    if (profesor == NULL) return 0;

    size_t largo = strlen(profesor);
    if (largo == 0) return 0;
    if (largo >= MAX_PROFESOR_LEN) return 0;

    int soloEspacios = 1;
    for (size_t i = 0; i < largo; i++) {
        unsigned char c = (unsigned char)profesor[i];
        if (c < 0x20 || c == 0x7F) return 0; /* caracteres de control */
        if (c != ' ' && c != '\t') soloEspacios = 0;
    }
    if (soloEspacios) return 0; /* "  " o "  \t" no son un nombre */

    return 1;
}

/*
 * Inicializa un grupo en un estado vacío y válido.
 *
 * Recibe: puntero al Grupo.
 * Devuelve: nada.
 */
void inicializarGrupo(Grupo *grupo) {
    if (grupo == NULL) return;
    grupo->numero = 0;
    grupo->tipo[0] = '\0';
    grupo->profesor[0] = '\0';
    grupo->cantidadHorarios = 0;
}

/*
 * Compara dos horarios y devuelve 1 si son idénticos (mismo día y horas),
 * o 0 si son distintos. Sirve para evitar horarios duplicados.
 */
int mismoHorario(const Horario *a, const Horario *b) {
    if (a == NULL || b == NULL) return 0;
    return a->horaInicio == b->horaInicio && a->horaFin == b->horaFin &&
           strcmp(a->dia, b->dia) == 0;
}

/*
 * Agrega un horario a un grupo, si hay espacio y no está repetido.
 *
 * Recibe: el Grupo y el Horario a agregar.
 * Devuelve:
 *   1 si se agregó correctamente,
 *   2 si el horario ya existía (no se agrega, no es error fatal),
 *   0 si no se pudo agregar (grupo lleno, puntero inválido u horario
 *     inválido).
 */
int agregarHorario(Grupo *grupo, const Horario *horario) {
    if (grupo == NULL || horario == NULL) return 0;
    if (validarHorario(horario) != 1) return 0;

    if (grupo->cantidadHorarios >= MAX_HORARIOS_POR_GRUPO) return 0;

    for (int i = 0; i < grupo->cantidadHorarios; i++) {
        if (mismoHorario(&grupo->horarios[i], horario)) return 2;
    }

    grupo->horarios[grupo->cantidadHorarios++] = *horario;
    return 1;
}

/*
 * Compara dos grupos completos (número, tipo, profesor y todos sus
 * horarios). Los horarios forman un conjunto: si ambos grupos tienen
 * exactamente los mismos horarios pero en distinto orden, se consideran
 * iguales. Devuelve 1 si son idénticos, 0 si difieren en algo.
 */
int gruposIguales(const Grupo *a, const Grupo *b) {
    if (a == NULL || b == NULL) return 0;
    if (a->numero != b->numero) return 0;
    if (a->cantidadHorarios != b->cantidadHorarios) return 0;
    if (strcmp(a->tipo, b->tipo) != 0) return 0;
    if (strcmp(a->profesor, b->profesor) != 0) return 0;

    /* Comparar horarios como conjunto: cada horario de `a` debe encontarse
     * en `b` una sola vez, sin importar el orden en que estén guardados. */
    int usado[MAX_HORARIOS_POR_GRUPO] = {0};
    for (int i = 0; i < a->cantidadHorarios; i++) {
        int encontrado = 0;
        for (int j = 0; j < b->cantidadHorarios; j++) {
            if (!usado[j] && mismoHorario(&a->horarios[i], &b->horarios[j])) {
                usado[j] = 1;
                encontrado = 1;
                break;
            }
        }
        if (!encontrado) return 0;
    }
    return 1;
}

/*
 * Valida un grupo. Se comprueba:
 *   - número de grupo mayor que 0 y dentro del límite del proyecto;
 *   - tipo dentro de los tipos que existen en data/cursos.csv;
 *   - profesor no vacío, de largo válido, sin ser solo espacios y sin
 *     caracteres de control;
 *   - al menos un horario, sin superar MAX_HORARIOS_POR_GRUPO;
 *   - sin horarios duplicados entre sí;
 *   - todos los horarios válidos (mediante validarHorario()).
 *
 * Recibe: puntero al Grupo.
 * Devuelve: 1 si es válido, 0 en caso contrario.
 */
int validarGrupo(const Grupo *grupo) {
    if (grupo == NULL) return 0;

    if (grupo->numero <= 0 || grupo->numero > MAX_NUMERO_GRUPO) return 0;

    if (!tipoGrupoValido(grupo->tipo)) return 0;

    if (!profesorValido(grupo->profesor)) return 0;

    if (grupo->cantidadHorarios < 1 ||
        grupo->cantidadHorarios > MAX_HORARIOS_POR_GRUPO) {
        return 0;
    }

    for (int i = 0; i < grupo->cantidadHorarios; i++) {
        if (validarHorario(&grupo->horarios[i]) != 1) return 0;
        for (int j = i + 1; j < grupo->cantidadHorarios; j++) {
            if (mismoHorario(&grupo->horarios[i], &grupo->horarios[j])) {
                return 0; /* horario duplicado dentro del grupo */
            }
        }
    }
    return 1;
}
