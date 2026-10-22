#ifndef REQUISITOS_H
#define REQUISITOS_H

/*
 * requisitos.h
 * 
 * Módulo: Requisitos
 * Propósito: Define la estructura Requisitos y funciones para manejar listas
 * de códigos de cursos (usada tanto para requisitos como para correquisitos).
 * 
 * Esta estructura almacena hasta MAX_REQUISITOS códigos de curso.
 */

#include "constantes.h"

/* Estructura que representa una lista de requisitos o correquisitos.
 * 
 * - codigos: Arreglo de códigos de cursos (ej.: "CE1101", "MA1102").
 * - cantidad: Número de códigos almacenados en la lista.
 */
typedef struct {
    char codigos[MAX_REQUISITOS][MAX_CODIGO_LEN];
    int cantidad;
} Requisitos;

/* Inicializa la lista de requisitos dejándola vacía.
 * 
 * Recibe: puntero a Requisitos.
 * Devuelve: nada.
 */
void inicializarRequisitos(Requisitos *req);

/* Verifica si un código ya existe en la lista.
 * 
 * Recibe: lista de requisitos y código a buscar.
 * Devuelve: 1 si el código existe, 0 si no existe.
 */
int contieneCodigo(const Requisitos *req, const char *codigo);

/* Compara dos listas de requisitos como conjuntos.
 * 
 * Son iguales si contienen exactamente los mismos códigos, sin importar
 * el orden en que estén almacenados.
 * 
 * Recibe: dos punteros a Requisitos.
 * Devuelve: 1 si son iguales, 0 si difieren.
 */
int requisitosIguales(const Requisitos *a, const Requisitos *b);

/* Agrega un código a la lista con validaciones.
 * 
 * Recibe: lista de requisitos y código a agregar.
 * Devuelve:
 *   0: agregado correctamente
 *   1: código vacío o formato inválido
 *   2: código demasiado largo (no se trunca)
 *   3: código duplicado (ya existe)
 *   4: máximo de requisitos alcanzado (MAX_REQUISITOS)
 */
int agregarCodigo(Requisitos *req, const char *codigo);

#endif
