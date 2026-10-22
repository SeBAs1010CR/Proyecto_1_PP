#ifndef CONSTANTES_H
#define CONSTANTES_H

/*
 * constantes.h
 * 
 * Módulo: Constantes compartidas del proyecto CEmestre
 * 
 * Propósito: Centraliza los límites y tamaños máximos que utilizan varios
 * módulos del proyecto. Esto evita definir constantes repetidas o dispersas
 * por todo el código. Las constantes exclusivamente internas a un único
 * archivo se mantienen dentro de ese mismo archivo.
 * 
 * Todas las constantes aquí definidas son usadas por las estructuras de datos
 * (Curso, Grupo, Horario, Requisitos, Historial) para reservar memoria de
 * forma segura y controlar los límites de validación.
 */

 /* Longitud máxima de un código de curso.
  * 
  * Los códigos del TEC tienen exactamente 6 caracteres (ej.: CE1101, MA1403).
  * Se reserva un poco más de espacio (20) para permitir holgura y evitar
  * desbordamientos al leer/almacenar cadenas. */
#define MAX_CODIGO_LEN 20

/* Cantidad máxima de requisitos o correquisitos por curso.
 * 
 * Cada curso puede tener hasta 10 códigos de cursos como requisitos
 * o como correquisitos. */
#define MAX_REQUISITOS 10

/* Cantidad máxima de horarios dentro de un grupo.
 * 
 * Un grupo puede tener hasta 10 bloques horarios diferentes. */
#define MAX_HORARIOS_POR_GRUPO 10

/* Cantidad máxima de grupos dentro de un curso.
 * 
 * Un mismo curso puede tener varios grupos (ej.: Grupo 1, Grupo 2...). */
#define MAX_GRUPOS_POR_CURSO 10

/* Tamaño máximo (en bytes) del nombre completo de un curso. */
#define MAX_NOMBRE_LEN 100

/* Tamaño máximo (en bytes) del nombre completo de un profesor. */
#define MAX_PROFESOR_LEN 100

/* Tamaño máximo (en bytes) del tipo de grupo.
 * 
 * Ejemplos: "Regular", "Semipresencial", "Virtual", "Asistida", "Grupo RN". */
#define MAX_TIPO_LEN 20

/* Tamaño máximo (en bytes) del día de un horario.
 * 
 * Se usa en formato normalizado de 3 letras: "LUN", "MAR", "MIE", "JUE",
 * "VIE", "SAB", "DOM". */
#define MAX_DIA_LEN 10

#endif
