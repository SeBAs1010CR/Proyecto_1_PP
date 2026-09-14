/*
 * requisitos.c
 *
 * Funciones para manejar listas de códigos (requisitos y correquisitos).
 * La misma estructura Requisitos se usa para ambos casos.
 */

#include <stdio.h>
#include <string.h>

#include "requisitos.h"

/*
 * Valida el formato de un código TEC: exactamente 6 caracteres, los 2
 * primeros son letras mayúsculas y los 4 últimos son números.
 * Ejemplos válidos: CE1101, MA1102, FI1201, FH0246, EM5001.
 *
 * Esta validación es privada de este archivo para no crear una dependencia
 * circular con curso.c (que ya depende de estos archivos), que tiene una
 * comprobación equivalente pero privada.
 *
 * Recibe: el código a validar.
 * Devuelve: 1 si el código es válido, 0 en caso contrario.
 */
static int codigoValido(const char *codigo) {
    if (codigo == NULL) return 0;
    if (strlen(codigo) != 6) return 0;

    for (int i = 0; i < 2; i++) {
        if (codigo[i] < 'A' || codigo[i] > 'Z') return 0;
    }
    for (int i = 2; i < 6; i++) {
        if (codigo[i] < '0' || codigo[i] > '9') return 0;
    }
    return 1;
}

/*
 * Indica si un código está vacío: puntero NULL, cadena vacía o compuesta
 * únicamente por espacios (o tabulaciones). Un "requisito vacío" dentro de
 * una lista siempre se considera inválido.
 *
 * Recibe: el código a revisar.
 * Devuelve: 1 si está vacío o solo contiene espacios, 0 en caso contrario.
 */
static int codigoVacio(const char *codigo) {
    if (codigo == NULL || codigo[0] == '\0') return 1;
    for (const char *p = codigo; *p != '\0'; p++) {
        if (*p != ' ' && *p != '\t') return 0;
    }
    return 1;
}

/*
 * Inicializa una lista de requisitos vacía (cantidad = 0).
 *
 * Recibe: puntero a Requisitos.
 * Devuelve: nada.
 */
void inicializarRequisitos(Requisitos *req) {
    if (req == NULL) return;
    req->cantidad = 0;
}

/*
 * Verifica si un código ya existe dentro de la lista.
 *
 * Recibe: la lista de Requisitos y el código a buscar.
 * Devuelve: 1 si el código aparece, 0 si no.
 */
int contieneCodigo(const Requisitos *req, const char *codigo) {
    if (req == NULL || codigo == NULL) return 0;
    for (int i = 0; i < req->cantidad; i++) {
        if (strcmp(req->codigos[i], codigo) == 0) return 1;
    }
    return 0;
}

/*
 * Compara dos listas de requisitos como un CONJUNTO: se consideran iguales
 * si contienen exactamente los mismos códigos, sin importar el orden en que
 * estén guardados. Devuelve 1 si son iguales, 0 si difieren en cantidad o
 * en algún código.
 */
int requisitosIguales(const Requisitos *a, const Requisitos *b) {
    if (a == NULL || b == NULL) return 0;
    if (a->cantidad != b->cantidad) return 0;
    if (a->cantidad == 0) return 1;

    /* Cada código de `a` debe encontrarse una sola vez en `b`, sin depender
     * del orden en que estén guardados. */
    int usado[MAX_REQUISITOS] = {0};
    for (int i = 0; i < a->cantidad; i++) {
        int encontrado = 0;
        for (int j = 0; j < b->cantidad; j++) {
            if (!usado[j] && strcmp(a->codigos[i], b->codigos[j]) == 0) {
                usado[j] = 1;
                encontrado = 1;
                break;
            }
        }
        if (!encontrado) return 0; /* falta, sobra o hay un código distinto */
    }
    return 1;
}

/*
 * Agrega un código a la lista, con validaciones.
 *
 * Recibe: la lista de Requisitos y el código a agregar.
 * Devuelve:
 *   0 si se agregó correctamente,
 *   1 si el código está vacío (NULL, "" o solo espacios) o tiene un
 *     formato inválido,
 *   2 si el código es demasiado largo (no se trunca),
 *   3 si el código ya existía (duplicado, no se agrega),
 *   4 si se alcanzó el máximo de requisitos permitido.
 */
int agregarCodigo(Requisitos *req, const char *codigo) {
    if (req == NULL) return 1;
    if (codigoVacio(codigo)) return 1;
    if (strlen(codigo) >= MAX_CODIGO_LEN) return 2;
    if (!codigoValido(codigo)) return 1;
    if (contieneCodigo(req, codigo)) return 3;
    if (req->cantidad >= MAX_REQUISITOS) return 4;

    strcpy(req->codigos[req->cantidad], codigo);
    req->cantidad++;
    return 0;
}
