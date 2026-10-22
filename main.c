/*
 * main.c
 *
 * Programa principal del proyecto CEmestre (Etapa 1).
 *
 * Orquesta las tres partes del proyecto en el orden en que dependen:
 *   1. Cargar el catálogo de cursos y el historial del estudiante.
 *   2. Detectar choques de horario entre cursos y grupos.
 *   3. Determinar la elegibilidad según requisitos y correquisitos.
 *   4. Exportar el catálogo a output/catalogo_salida.csv y verificarlo
 *      en consola.
 *
 * Debe ejecutarse desde la raíz del proyecto (las rutas de los archivos
 * de datos y de salida son relativas a ese directorio).
 *
 * Compilación:
 *   gcc -Iinclude -Wall -std=c11 main.c src/curso.c src/grupo.c \
 *       src/horario.c src/requisitos.c src/historial.c src/conflictos.c \
 *       src/elegibilidad.c src/salida.c -o main
 */

#include <stdio.h>
#include <string.h>

#include "curso.h"
#include "historial.h"
#include "conflictos.h"
#include "elegibilidad.h"
#include "salida.h"

/* Capacidad del catálogo en memoria. El límite interno de src/curso.c es 70;
 * aquí se reserva un poco más para que el programa principal pueda manejar
 * el catálogo completo (y su crecimiento) sin truncar datos. */
#define MAX_CURSOS 100

/* Curso del catálogo con su valor de elegibilidad esperado, usado para
 * comprobar en consola que las reglas se están aplicando. */
typedef struct {
    const char *codigo;
    int esperado;
    const char *descripcion;
} CasoPrueba;

static const CasoPrueba CASOS_PRUEBA[] = {
    {"CE1103", 1, "sin requisitos, no aprobado: elegible"},
    {"CE1105", 1, "sin requisitos, no aprobado: elegible"},
    {"FI1102", 1, "requisitos aprobados: elegible"},
    {"CE2103", 0, "requisitos faltantes: bloqueado"},
    {"MA2104", 0, "requisitos faltantes: bloqueado"},
    {"CE1101", 0, "ya aprobado en el historial: no se repite"},
    {"QU1102", 1, "correquisito mutuo con QU1106: elegible"},
    {"QU1106", 1, "correquisito mutuo con QU1102: elegible"},
};

static Curso cursos[MAX_CURSOS];
static int choques[MAX_CURSOS];
static Historial historial;

/*
 * buscarCursoPorCodigo
 *
 * Busca un curso del catálogo por su código.
 *
 * Recibe: cantidad de cursos y código buscado.
 * Devuelve: puntero al curso encontrado, o NULL si no está en el catálogo.
 */
static const Curso *buscarCursoPorCodigo(int cantidad, const char *codigo) {
    for (int i = 0; i < cantidad; i++) {
        if (strcmp(cursos[i].codigo, codigo) == 0) return &cursos[i];
    }
    return NULL;
}

/*
 * imprimirResumen
 *
 * Muestra por consola un resumen de los resultados calculados: grupos por
 * curso, cursos con choque de horario y cursos según su elegibilidad.
 *
 * Recibe: cantidad de cursos.
 * Devuelve: nada.
 */
static void imprimirResumen(int cantidad) {
    int totalGrupos = 0;
    int conChoque = 0;
    int elegibles = 0;
    int bloqueados = 0;
    int aprobados = 0;

    for (int i = 0; i < cantidad; i++) {
        totalGrupos += cursos[i].cantidadGrupos;
        if (choques[i]) conChoque++;
        if (cursoAprobado(&historial, cursos[i].codigo)) {
            aprobados++;
        } else if (cursos[i].puedeMatricular) {
            elegibles++;
        } else {
            bloqueados++;
        }
    }

    printf("\n=== Resumen de resultados ===\n");
    printf("Cursos en el catalogo   : %d\n", cantidad);
    printf("Grupos totales         : %d\n", totalGrupos);
    printf("Cursos que chocan      : %d\n", conChoque);
    printf("Cursos elegibles       : %d\n", elegibles);
    printf("Cursos bloqueados      : %d\n", bloqueados);
    printf("Cursos ya aprobados    : %d\n", aprobados);
}

/*
 * listarElegibles
 *
 * Imprime los códigos de los cursos que el estudiante puede matricular.
 *
 * Recibe: cantidad de cursos.
 * Devuelve: nada.
 */
static void listarElegibles(int cantidad) {
    printf("\nCursos que SI puede matricular:\n");
    int encontrados = 0;
    for (int i = 0; i < cantidad; i++) {
        if (cursos[i].puedeMatricular) {
            printf("  %s  %s\n", cursos[i].codigo, cursos[i].nombre);
            encontrados++;
        }
    }
    if (encontrados == 0) printf("  (ninguno)\n");
}

/*
 * verificarCasos
 *
 * Revisa casos conocidos del catálogo y del historial para confirmar que
 * las reglas de elegibilidad y de correquisitos se aplican correctamente.
 *
 * Recibe: cantidad de cursos.
 * Devuelve: cantidad de casos que NO cumplieron lo esperado.
 */
static int verificarCasos(int cantidad) {
    printf("\n=== Verificacion de casos ===\n");
    int fallos = 0;
    int total = (int)(sizeof(CASOS_PRUEBA) / sizeof(CASOS_PRUEBA[0]));

    for (int i = 0; i < total; i++) {
        const CasoPrueba *caso = &CASOS_PRUEBA[i];
        const Curso *curso = buscarCursoPorCodigo(cantidad, caso->codigo);

        if (curso == NULL) {
            printf("  [AVISO] %s no esta en el catalogo (omitido).\n",
                   caso->codigo);
            continue;
        }

        int obtenido = curso->puedeMatricular ? 1 : 0;
        if (obtenido == caso->esperado) {
            printf("  [OK]   %s = %d  (%s)\n", caso->codigo, obtenido,
                   caso->descripcion);
        } else {
            printf("  [FALLA] %s = %d, se esperaba %d  (%s)\n",
                   caso->codigo, obtenido, caso->esperado, caso->descripcion);
            fallos++;
        }
    }
    return fallos;
}

/*
 * mostrarEjemploSalida
 *
 * Muestra las primeras líneas del archivo generado para revisar el formato
 * sin abrir el archivo.
 *
 * Recibe: nada.
 * Devuelve: nada.
 */
static void mostrarEjemploSalida(void) {
    FILE *archivo = fopen("output/catalogo_salida.csv", "r");
    if (archivo == NULL) {
        fprintf(stderr, "[Aviso] No se pudo leer el archivo de salida.\n");
        return;
    }

    printf("\n=== Primeras lineas de output/catalogo_salida.csv ===\n");
    char linea[1024];
    for (int i = 0; i < 6 && fgets(linea, sizeof(linea), archivo) != NULL;
         i++) {
        printf("  %s", linea);
    }

    fclose(archivo);
}

/*
 * main
 *
 * Carga los datos, calcula choques y elegibilidad, exporta el catálogo y
 * muestra en consola la verificación de los resultados.
 *
 * Devuelve: 0 si todo el proceso fue exitoso, 1 si falló algún paso.
 */
int main(void) {
    printf("=== CEmestre: Etapa 1 ===\n");

    int cantidad = cargarCursos(cursos, MAX_CURSOS);
    if (cantidad < 0) {
        fprintf(stderr, "[Error] No se pudo cargar el catalogo de cursos.\n");
        return 1;
    }
    if (cantidad == 0) {
        fprintf(stderr, "[Error] El catalogo de cursos esta vacio.\n");
        return 1;
    }

    int aprobados = cargarHistorial(&historial);
    if (aprobados < 0) {
        fprintf(stderr, "[Error] No se pudo cargar el historial.\n");
        return 1;
    }
    printf("\nHistorial: %d curso(s) aprobado(s) cargado(s).\n", aprobados);

    for (int i = 0; i < cantidad; i++) {
        choques[i] = 0;
    }
    detectarChoques(cursos, cantidad, choques);

    determinarElegibilidad(cursos, cantidad, &historial);

    imprimirResumen(cantidad);
    listarElegibles(cantidad);

    int lineas = exportarCatalogo(cursos, cantidad, choques);
    if (lineas < 0) {
        fprintf(stderr, "[Error] No se pudo exportar el catalogo.\n");
        return 1;
    }

    mostrarEjemploSalida();

    int fallos = verificarCasos(cantidad);
    if (fallos > 0) {
        printf("\nResultado: %d verificacion(es) fallida(s).\n", fallos);
        return 1;
    }

    printf("\nResultado: proceso completo sin fallos.\n");
    return 0;
}