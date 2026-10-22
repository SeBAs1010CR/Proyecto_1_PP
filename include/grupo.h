#ifndef GRUPO_H
#define GRUPO_H

/*
 * grupo.h
 * 
 * Módulo: Grupo
 * Propósito: Define la estructura Grupo y funciones para manipular grupos
 * (secciones) de un curso. Cada grupo tiene horarios, profesor, tipo y número.
 */

#include "horario.h"
#include "constantes.h"

/* Estructura que representa un grupo (sección) de un curso.
 * 
 * - numero: Número de grupo (ej.: 1, 2, 3...).
 * - tipo: Tipo de grupo ("Regular", "Semipresencial", "Virtual", 
 *          "Asistida", "Grupo RN").
 * - profesor: Nombre del profesor que imparte este grupo.
 * - horarios: Arreglo de horarios del grupo.
 * - cantidadHorarios: Cantidad de horarios asignados al grupo.
 */
typedef struct {
    int numero;
    char tipo[MAX_TIPO_LEN];
    char profesor[MAX_PROFESOR_LEN];
    Horario horarios[MAX_HORARIOS_POR_GRUPO];
    int cantidadHorarios;
} Grupo;

/* Inicializa un grupo en estado vacío.
 * 
 * Recibe: puntero a Grupo.
 * Devuelve: nada.
 */
void inicializarGrupo(Grupo *grupo);

/* Agrega un horario al grupo si hay espacio y no está repetido.
 * 
 * Recibe: grupo y horario a agregar.
 * Devuelve: 1 si se agregó correctamente, 2 si el horario ya existía
 * (no se agrega), 0 si no se pudo agregar (grupo lleno, puntero inválido
 * u horario inválido).
 */
int agregarHorario(Grupo *grupo, const Horario *horario);

/* Compara dos horarios para determinar si son idénticos.
 * 
 * Recibe: dos punteros a Horario.
 * Devuelve: 1 si son idénticos (mismo día, misma hora inicio y fin), 0 si no.
 */
int mismoHorario(const Horario *a, const Horario *b);

/* Compara dos grupos completos para ver si son idénticos.
 * 
 * Compara número, tipo, profesor, cantidad y horarios (como conjunto,
 * sin importar orden).
 * 
 * Recibe: dos punteros a Grupo.
 * Devuelve: 1 si son idénticos, 0 si difieren.
 */
int gruposIguales(const Grupo *a, const Grupo *b);

/* Valida que un grupo tenga datos correctos y coherentes.
 * 
 * Verifica: número válido, tipo válido, profesor válido, cantidad de horarios
 * válida (>=1), sin horarios duplicados, todos los horarios válidos.
 * 
 * Recibe: puntero a Grupo.
 * Devuelve: 1 si es válido, 0 en caso contrario.
 */
int validarGrupo(const Grupo *grupo);

#endif
