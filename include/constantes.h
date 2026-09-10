#ifndef CONSTANTES_H
#define CONSTANTES_H

/* Constantes generales/compartidas del proyecto.
 *
 * Centralizan los límites y tamaños que varios módulos utilizan, para
 * evitar definiciones repetidas o dispersas. Las constantes internas de
 * un solo archivo permanecen en su propio módulo. */

/* Longitud máxima de un código de curso (el código TEC ocupa 6 caracteres,
 * pero el campo reservado admite holgura). */
#define MAX_CODIGO_LEN 20

/* Cantidad máxima de requisitos (o correquisitos) por curso. */
#define MAX_REQUISITOS 10

/* Cantidad máxima de horarios dentro de un grupo. */
#define MAX_HORARIOS_POR_GRUPO 10

/* Cantidad máxima de grupos dentro de un curso. */
#define MAX_GRUPOS_POR_CURSO 10

/* Tamaño máximo (en bytes) del nombre de un curso. */
#define MAX_NOMBRE_LEN 100

/* Tamaño máximo (en bytes) del nombre de un profesor. */
#define MAX_PROFESOR_LEN 100

/* Tamaño máximo (en bytes) del tipo de grupo ("Semipresencial"). */
#define MAX_TIPO_LEN 20

/* Tamaño máximo (en bytes) del día de un horario ("MIE"). */
#define MAX_DIA_LEN 10

#endif
