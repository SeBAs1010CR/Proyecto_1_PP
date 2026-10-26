#ifndef CURSO_H
#define CURSO_H

/*
 * curso.h
 * 
 * Módulo: Curso
 * Propósito: Define la estructura Curso y declara funciones para cargar,
 * validar y manipular el catálogo de cursos desde el archivo CSV.
 * 
 * Cada Curso representa una materia (asignatura) y contiene toda su información:
 * código, nombre, créditos, horas, requisitos, correquisitos, y sus grupos.
 */

#include "grupo.h"
#include "requisitos.h"
#include "constantes.h"

/* Estructura que representa un curso del catálogo.
 * 
 * - codigo: Código del curso (ej.: "CE1101"). Formato TEC: 2 letras + 4 números.
 * - nombre: Nombre completo de la materia.
 * - creditos: Número de créditos del curso.
 * - horas: Número de horas totales del curso.
 * - requisitos: Lista de códigos de cursos que son requisitos para este curso.
 * - correquisitos: Lista de códigos de cursos que son correquisitos.
 * - grupos: Arreglo de grupos que imparten este curso.
 * - cantidadGrupos: Número de grupos cargados para este curso.
 * - puedeMatricular: Indicador de elegibilidad (1 = puede matricularse, 0 = no).
 * - carrera: Carrera que aporta el curso (columna 12 de data/cursos.csv).
 */
typedef struct {
    char codigo[MAX_CODIGO_LEN];
    char nombre[MAX_NOMBRE_LEN];
    int creditos;
    int horas;
    Requisitos requisitos;
    Requisitos correquisitos;
    Grupo grupos[MAX_GRUPOS_POR_CURSO];
    int cantidadGrupos;
    int puedeMatricular; /* 1 si el estudiante puede matricularlo, 0 si no */
    char carrera[MAX_CARRERA_LEN]; /* carrera que aporta el curso */
} Curso;

/* Inicializa un curso en estado vacío.
 * 
 * Recibe: puntero a Curso a inicializar.
 * Devuelve: nada.
 */
void inicializarCurso(Curso *curso);

/* Parsea una línea del CSV y llena la estructura Curso con sus datos.
 * 
 * Recibe: línea de texto del archivo CSV y puntero a Curso de salida.
 * Devuelve: 0 si fue exitoso, o un código negativo indicando el tipo de error:
 *   -1: registro incompleto (< 12 campos)
 *   -12: más de 12 campos
 *   -14: carrera vacía o demasiado larga
 *   -13: campo demasiado largo (sin truncar)
 *   -2: código inválido
 *   -3: nombre inválido
 *   -4: número de grupo inválido
 *   -5: tipo de grupo inválido
 *   -6: profesor inválido
 *   -7: horario mal formado
 *   -8: créditos inválidos
 *   -9: horas inválidas
 *   -10: requisitos mal formados
 *   -11: correquisitos mal formados
 */
int leerCurso(const char *linea, Curso *curso);

/* Carga todos los cursos desde el archivo data/cursos.csv.
 * 
 * Agrupa líneas con el mismo código en un solo Curso (cada línea es un grupo).
 * Realiza validaciones, detecta duplicados e inconsistencias, y verifica
 * que requisitos/correquisitos apunten a cursos existentes.
 * 
 * Recibe: arreglo de Curso donde cargar y su capacidad máxima (maxCursos).
 * Devuelve: cantidad de cursos cargados, o -1 si no se pudo abrir el archivo
 * o si los parámetros son inválidos.
 */
int cargarCursos(Curso *cursos, int maxCursos);

#endif
