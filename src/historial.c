/*
 * historial.c
 *
 * Funciones para cargar el historial de cursos aprobados desde
 * data/historial.csv (un código de curso por línea) y para consultarlo.
 */

#include <stdio.h>
#include <string.h>

#include "historial.h"

#define RUTA_HISTORIAL "data/historial.csv"
#define TAM_LINEA 256

/* Quita espacios y saltos de línea al inicio y al final de una cadena. */
static void recortar(char *s) {
    if (s == NULL) return;
    char *inicio = s;
    while (*inicio == ' ' || *inicio == '\t' || *inicio == '\r' ||
           *inicio == '\n') {
        inicio++;
    }
    if (inicio != s) memmove(s, inicio, strlen(inicio) + 1);

    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == ' ' || s[n - 1] == '\t' ||
                     s[n - 1] == '\r' || s[n - 1] == '\n')) {
        s[--n] = '\0';
    }
}

/*
 * Lee data/historial.csv y carga los cursos aprobados en el Historial.
 * Si el archivo no existe, deja el historial vacío y avisa.
 *
 * Recibe: puntero al Historial donde cargar.
 * Devuelve: la cantidad de cursos aprobados cargados (0 si el archivo no
 * existe o está vacío), o -1 si `historial` es un puntero nulo.
 */
int cargarHistorial(Historial *historial) {
    if (historial == NULL) return -1;
    historial->cantidadCursos = 0;

    FILE *archivo = fopen(RUTA_HISTORIAL, "r");
    if (archivo == NULL) {
        fprintf(stderr,
                "[Aviso] No existe el archivo %s; el historial queda vacío.\n",
                RUTA_HISTORIAL);
        return 0;
    }

    char linea[TAM_LINEA];
    int numLinea = 0;
    int nIgnoradas = 0;

    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        numLinea++;
        recortar(linea);
        if (linea[0] == '\0') continue; /* línea en blanco, se ignora */

        if (strlen(linea) >= MAX_CODIGO_LEN) {
            fprintf(stderr,
                    "[Error en línea %d] Código demasiado largo ('%s'); "
                    "se omite.\n",
                    numLinea, linea);
            nIgnoradas++;
            continue;
        }

        int duplicado = 0;
        for (int i = 0; i < historial->cantidadCursos; i++) {
            if (strcmp(historial->codigos[i], linea) == 0) {
                duplicado = 1;
                break;
            }
        }
        if (duplicado) {
            fprintf(stderr,
                    "[Línea %d] Código duplicado ('%s'); se omite para no "
                    "cargarlo dos veces.\n",
                    numLinea, linea);
            nIgnoradas++;
            continue;
        }

        if (historial->cantidadCursos >= MAX_CURSOS_APROBADOS) {
            fprintf(stderr,
                    "[Error en línea %d] Se alcanzó el máximo de %d cursos "
                    "aprobados; se omite '%s'.\n",
                    numLinea, MAX_CURSOS_APROBADOS, linea);
            nIgnoradas++;
            continue;
        }

        strcpy(historial->codigos[historial->cantidadCursos], linea);
        historial->cantidadCursos++;
    }

    fclose(archivo);

    printf("Historial cargado: %d cursos aprobados (%d líneas ignoradas).\n",
           historial->cantidadCursos, nIgnoradas);
    return historial->cantidadCursos;
}

/*
 * Determina si un curso aparece dentro del historial.
 *
 * Recibe: el Historial y el código del curso a buscar.
 * Devuelve: 1 si el curso está aprobado en el historial, 0 si no lo está.
 */
int cursoAprobado(const Historial *historial, const char *codigo) {
    if (historial == NULL || codigo == NULL) return 0;
    for (int i = 0; i < historial->cantidadCursos; i++) {
        if (strcmp(historial->codigos[i], codigo) == 0) return 1;
    }
    return 0;
}
