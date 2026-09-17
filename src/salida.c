#include <stdio.h>

#include "salida.h"

#define RUTA_SALIDA "output/catalogo_salida.csv"
#define ENCABEZADO "codigo;nombre;creditos;grupo;tipo;profesor;horarios;" \
                   "requisitos;correquisitos;choca;puede_matricular\n"

/* Escribe los horarios del grupo separados por '|': MIE 07:30-09:20|... */
static void escribirHorarios(FILE *archivo, const Grupo *grupo) {
    for (int i = 0; i < grupo->cantidadHorarios; i++) {
        const Horario *h = &grupo->horarios[i];
        if (i > 0) fputc('|', archivo);
        fprintf(archivo, "%s %02d:%02d-%02d:%02d", h->dia,
                h->horaInicio / 100, h->horaInicio % 100,
                h->horaFin / 100, h->horaFin % 100);
    }
}

/* Escribe una lista de códigos separados por coma. */
static void escribirCodigos(FILE *archivo, const Requisitos *lista) {
    for (int i = 0; i < lista->cantidad; i++) {
        if (i > 0) fputc(',', archivo);
        fputs(lista->codigos[i], archivo);
    }
}

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
            fprintf(archivo, ";%d;%d\n", choca,
                    curso->puedeMatricular ? 1 : 0);
            lineas++;
        }
    }

    fclose(archivo);
    printf("Catálogo exportado a %s (%d líneas).\n", RUTA_SALIDA, lineas);
    return lineas;
}
