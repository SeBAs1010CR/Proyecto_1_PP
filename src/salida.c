/*
 * salida.c
 * 
 * Módulo: Exportación de resultados
 * Autoría: Persona 3 - Bryan Sibaja
 * 
 * Propósito: Exporta el catálogo de cursos a output/catalogo_salida.csv.
 * Formato: CSV separado por ';', una línea por cada grupo de cada curso.
 * Este formato es el que consumirá la etapa 2 (Racket).
 */

#include <stdio.h>

#include "salida.h"

#define RUTA_SALIDA "output/catalogo_salida.csv"
#define ENCABEZADO "codigo;nombre;creditos;grupo;tipo;profesor;horarios;" \
                   "requisitos;correquisitos;choca;puede_matricular;carrera\n"

/*
 * escribirHorarios
 * 
 * Escribe los horarios del grupo en formato para CSV de salida.
 * Formato: bloques separados por '|' (ej.: "MIE 07:30-09:20|VIE 07:30-09:20")
 * Cada bloque: día (3 letras) + espacio + horaInicio-HoraFin con formato HH:MM
 * 
 * Recibe: archivo abierto para escritura y puntero a grupo.
 * Devuelve: nada.
 */
static void escribirHorarios(FILE *archivo, const Grupo *grupo) {
    for (int i = 0; i < grupo->cantidadHorarios; i++) {
        const Horario *h = &grupo->horarios[i];
        if (i > 0) fputc('|', archivo);
        fprintf(archivo, "%s %02d:%02d-%02d:%02d", h->dia,
                h->horaInicio / 100, h->horaInicio % 100,
                h->horaFin / 100, h->horaFin % 100);
    }
}

/*
 * escribirCodigos
 * 
 * Escribe lista de códigos separados por coma (sin espacios).
 * Si la lista está vacía, no escribe nada.
 * Ejemplo: "CE1101,MA1102" o "" (cadena vacía).
 * 
 * Recibe: archivo y lista de requisitos/correquisitos.
 * Devuelve: nada.
 */
static void escribirCodigos(FILE *archivo, const Requisitos *lista) {
    for (int i = 0; i < lista->cantidad; i++) {
        if (i > 0) fputc(',', archivo);
        fputs(lista->codigos[i], archivo);
    }
}

/*
 * exportarCatalogo
 * 
 * Abre (crea/sobrescribe) output/catalogo_salida.csv y escribe el encabezado
 * seguido de una línea por cada grupo de cada curso.
 * El valor 'choca' es el mismo para todos los grupos del curso.
 * 
 * Recibe: cursos, cantidadCursos, arreglo paralelo choques (puede ser NULL).
 * Devuelve: cantidad de líneas escritas, o -1 si hubo error.
 */
int exportarCatalogo(const Curso *cursos, int cantidadCursos,
                     const int *choques) {
    if (cursos == NULL || cantidadCursos < 0) return -1;

    FILE *archivo = fopen(RUTA_SALIDA, "w");
    if (archivo == NULL) {
        fprintf(stderr, "[Error] No se pudo crear el archivo %s.\n",
                RUTA_SALIDA);
        return -1;
    }

    fputs(ENCABEZADO, archivo);

    int lineas = 0;
    for (int i = 0; i < cantidadCursos; i++) {
        const Curso *curso = &cursos[i];
        int choca = (choques != NULL && choques[i]) ? 1 : 0;

        for (int j = 0; j < curso->cantidadGrupos; j++) {
            const Grupo *grupo = &curso->grupos[j];

            fprintf(archivo, "%s;%s;%d;%d;%s;%s;", curso->codigo,
                    curso->nombre, curso->creditos, grupo->numero,
                    grupo->tipo, grupo->profesor);
            escribirHorarios(archivo, grupo);
            fputc(';', archivo);
            escribirCodigos(archivo, &curso->requisitos);
            fputc(';', archivo);
            escribirCodigos(archivo, &curso->correquisitos);
            fprintf(archivo, ";%d;%d;%s\n", choca,
                    curso->puedeMatricular ? 1 : 0, curso->carrera);
            lineas++;
        }
    }

    fclose(archivo);
    printf("Catálogo exportado a %s (%d líneas).\n", RUTA_SALIDA, lineas);
    return lineas;
}
