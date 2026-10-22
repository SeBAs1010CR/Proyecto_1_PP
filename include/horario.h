#ifndef HORARIO_H
#define HORARIO_H

/*
 * horario.h
 * 
 * Módulo: Horario
 * Propósito: Define la estructura Horario y funciones para parsear, interpretar
 * y validar bloques horarios desde el formato del CSV.
 * 
 * Formatos soportados:
 * - "MIE[07:30-09:20], VIE[07:30-09:20]" (día en mayúsculas + corchetes)
 * - "Mié 18:00-20:50 / Vie 18:00-19:50" (día con tilde + espacio)
 * 
 * Las horas se representan internamente como enteros en formato HHMM
 * (ej.: 07:30 → 730, 09:20 → 920).
 */

#include "constantes.h"

/* Estructura que representa un bloque horario.
 * 
 * - dia: Día de la semana en formato normalizado: "LUN", "MAR", "MIE", 
 *        "JUE", "VIE", "SAB", "DOM".
 * - horaInicio: Hora de inicio en formato HHMM (ej.: 730 para 7:30).
 * - horaFin: Hora de finalización en formato HHMM (ej.: 920 para 9:20).
 */
typedef struct {
    char dia[MAX_DIA_LEN];
    int horaInicio;
    int horaFin;
} Horario;

/* Parsea un solo bloque de horario.
 * 
 * Ejemplos de bloque: "MIE[07:30-09:20]" o "Mié 18:00-20:50"
 * También acepta guion largo Unicode U+2013 como separador.
 * 
 * Recibe: cadena con el bloque y puntero a Horario de salida.
 * Devuelve: 1 si es válido, 0 en caso contrario.
 */
int parsearHorario(const char *bloque, Horario *horario);

/* Parsea un campo completo de horarios (puede tener varios bloques separados).
 * 
 * Separadores aceptados: coma (,) o barra (/).
 * Ejemplos: "MIE[07:30-09:20], VIE[07:30-09:20]" o "Mié 18:00-20:50 / Vie 18:00-19:50"
 * 
 * Recibe: texto del campo, arreglo de Horarios y capacidad máxima.
 * Devuelve: cantidad de horarios cargados (>=1), -1 si hay más de los que caben,
 * -2 si el formato es incorrecto o inválido.
 */
int parsearHorarios(const char *texto, Horario *horarios, int maxHorarios);

/* Valida que un horario sea correcto.
 * 
 * Verifica: día válido, horas válidas (00-23, 00-59), horaInicio < horaFin.
 * 
 * Recibe: puntero a Horario.
 * Devuelve: 1 si es válido, 0 en caso contrario.
 */
int validarHorario(const Horario *horario);

#endif
