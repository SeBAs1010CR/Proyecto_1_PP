#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "curso.h"

#define MAX_CURSOS 70
#define MAX_CAMPOS 11
#define TAM_CAMPO 320
#define TAM_LINEA 2048
#define RUTA_CURSOS "data/cursos.csv"

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

/* Retorna la longitud de s, sin sobrepasar `limite`. */
static size_t longitudSegura(const char *s, size_t limite) {
    size_t n = 0;
    if (s == NULL) return 0;
    while (n < limite && s[n] != '\0') n++;
    return n;
}

/*
 * Valida el formato de un código TEC: exactamente 6 caracteres, con 2 letras
 * mayúsculas seguidas de 4 números. Ejemplos: CE1101, MA0101, EM5001.
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
 * Divide una línea del CSV en campos separados por ';'. Cada campo queda
 * recortado (sin espacios) y copiado dentro de `campos`.
 *
 * Recibe: la línea y un arreglo donde dejar los campos.
 * Devuelve:
 *   - la cantidad de campos encontrados (0..MAX_CAMPOS) si la línea es válida,
 *   - -1 si hay más campos de los que caben,
 *   - -2 si algún campo excede TAM_CAMPO (no se trunca información, la línea
 *      se marca como inválida para que el registro sea rechazado).
 */
static int dividirLinea(const char *linea,
                        char campos[MAX_CAMPOS][TAM_CAMPO]) {
    if (linea == NULL) return 0;

    int n = 0;
    const char *p = linea;
    while (n < MAX_CAMPOS) {
        const char *fin = p;
        while (*fin && *fin != ';') fin++;

        size_t len = (size_t)(fin - p);
        if (len > TAM_CAMPO - 1) {
            /* No truncar: copiar solo un fragmento seguro para diagnóstico y
             * devolver un error para que leerCurso rechace el registro. */
            memset(campos[n], 0, TAM_CAMPO);
            memcpy(campos[n], p, TAM_CAMPO - 1);
            return -2;
        }

        memset(campos[n], 0, TAM_CAMPO);
        memcpy(campos[n], p, len);
        campos[n][len] = '\0';
        recortar(campos[n]);
        n++;

        /* Si el campo terminó en el final de la línea, la cantidad actual
         * es la correcta (los campos vacíos al final ya se contaron). */
        if (*fin == '\0') return n;
        p = fin + 1;
    }

    /* Se llenaron los MAX_CAMPOS y aún quedaban más campos por leer. */
    return -1;
}

/*
 * Configura un curso vacío (código, nombre, créditos, horas, requisitos,
 * correquisitos y cantidad de grupos en cero).
 *
 * Recibe: puntero al Curso.
 * Devuelve: nada.
 */
void inicializarCurso(Curso *curso) {
    if (curso == NULL) return;
    curso->codigo[0] = '\0';
    curso->nombre[0] = '\0';
    curso->creditos = 0;
    curso->horas = 0;
    inicializarRequisitos(&curso->requisitos);
    inicializarRequisitos(&curso->correquisitos);
    curso->cantidadGrupos = 0;
    curso->puedeMatricular = 0;
}

/*
 * Parsea un campo de códigos separados por coma (requisitos o correquisitos)
 * y los agrega a una estructura Requisitos.
 *
 * Reglas:
 *   - un campo completamente vacío es válido (no hay requisitos);
 *   - no se permiten códigos vacíos dentro de la lista;
 *   - no se permiten códigos duplicados;
 *   - no se permiten códigos demasiado largos;
 *   - todo código debe tener el formato TEC de codigoValido();
 *   - respeta el máximo MAX_REQUISITOS.
 *
 * Recibe: el texto del campo y la lista donde cargar.
 * Devuelve: 1 si el campo es válido, 0 si está mal formado.
 */
static int parsearCodigos(const char *texto, Requisitos *req) {
    inicializarRequisitos(req);

    /* Campo vacío = no hay requisitos/correquisitos. */
    if (texto == NULL || texto[0] == '\0') return 1;

    char buffer[TAM_CAMPO];
    snprintf(buffer, sizeof(buffer), "%s", texto);
    size_t len = strlen(buffer);

    size_t i = 0;
    while (i <= len) {
        /* Buscar el fin del token: una coma o el final de la cadena.
         * Se divide manualmente (sin strtok) para poder detectar códigos
         * vacíos que strtok saltaría silenciosamente. */
        size_t fin = i;
        while (fin < len && buffer[fin] != ',') fin++;

        size_t tl = fin - i;
        if (tl >= MAX_CODIGO_LEN) return 0; /* código demasiado largo */

        char codigo[MAX_CODIGO_LEN];
        memcpy(codigo, buffer + i, tl);
        codigo[tl] = '\0';
        recortar(codigo);

        if (codigo[0] == '\0') return 0;        /* código vacío en la lista */
        if (!codigoValido(codigo)) return 0;    /* formato inválido */
        if (agregarCodigo(req, codigo) != 0) {  /* duplicado o máximo */
            return 0;
        }

        if (fin == len) break;
        i = fin + 1;
    }
    return 1;
}

/*
 * Convierte el código de error de leerCurso en un mensaje legible.
 *
 * Recibe: código de error negativo.
 * Devuelve: cadena de texto con la descripción del error.
 */
static const char *textoErrorLeerCurso(int codigo) {
    switch (codigo) {
        case -1: return "Registro incompleto o con separador incorrecto (se esperaban 11 campos con ';').";
        case -12: return "Registro con más de 11 campos.";
        case -13: return "Campo demasiado largo (excede el tamaño máximo permitido); no se trunca información.";
        case -2: return "Código de curso inválido (debe tener 2 letras mayúsculas y 4 números, p. ej. CE1101).";
        case -3: return "Nombre de curso vacío o demasiado largo.";
        case -4: return "Número de grupo inválido.";
        case -5: return "Tipo de grupo vacío o demasiado largo.";
        case -6: return "Profesor vacío o demasiado largo.";
        case -7: return "Horario mal formado (día, hora o exceso de horarios).";
        case -8: return "Créditos inválidos.";
        case -9: return "Horas inválidas.";
        case -10: return "Requisitos mal formados (código vacío, duplicado, demasiado largo, formato inválido o exceso de código).";
        case -11: return "Correquisitos mal formados (código vacío, duplicado, demasiado largo, formato inválido o exceso de código).";
        default: return "Error desconocido.";
    }
}

/*
 * Parsea UNA línea del CSV y deja en `curso` toda la información del curso,
 * incluyendo su primer grupo (grupos[0]) con sus horarios.
 *
 * Recibe: la línea de texto y un puntero al Curso de salida.
 * Devuelve: 0 si fue correcto, o un código negativo indicando el error.
 */
int leerCurso(const char *linea, Curso *curso) {
    char campos[MAX_CAMPOS][TAM_CAMPO];
    int n = dividirLinea(linea, campos);

    if (n == -1) return -12;   /* más de 11 campos */
    if (n == -2) return -13;   /* un campo excede el máximo, sin truncar */
    if (n < MAX_CAMPOS) return -1;

    inicializarCurso(curso);

    /* codigo */
    if (campos[0][0] == '\0') return -2;
    if (!codigoValido(campos[0])) return -2;
    snprintf(curso->codigo, sizeof(curso->codigo), "%s", campos[0]);

    /* nombre */
    if (campos[1][0] == '\0') return -3;
    if (longitudSegura(campos[1], MAX_NOMBRE_LEN) >= MAX_NOMBRE_LEN) return -3;
    snprintf(curso->nombre, sizeof(curso->nombre), "%s", campos[1]);

    /* créditos */
    char *fin = NULL;
    long v = strtol(campos[6], &fin, 10);
    if (fin == campos[6] || *fin != '\0' || v < 0 || v > 3000) return -8;
    curso->creditos = (int)v;

    /* horas */
    v = strtol(campos[7], &fin, 10);
    if (fin == campos[7] || *fin != '\0' || v < 0 || v > 3000) return -9;
    curso->horas = (int)v;

    /* grupo */
    inicializarGrupo(&curso->grupos[0]);
    v = strtol(campos[2], &fin, 10);
    if (fin == campos[2] || *fin != '\0' || v <= 0 || v > 9999) return -4;
    curso->grupos[0].numero = (int)v;

    /* tipo */
    if (campos[3][0] == '\0') return -5;
    if (longitudSegura(campos[3], MAX_TIPO_LEN) >= MAX_TIPO_LEN) return -5;
    snprintf(curso->grupos[0].tipo, sizeof(curso->grupos[0].tipo), "%s",
             campos[3]);

    /* profesor */
    if (campos[4][0] == '\0') return -6;
    if (longitudSegura(campos[4], MAX_PROFESOR_LEN) >= MAX_PROFESOR_LEN) {
        return -6;
    }
    snprintf(curso->grupos[0].profesor, sizeof(curso->grupos[0].profesor),
             "%s", campos[4]);

    /* horarios */
    int cant = parsearHorarios(campos[5], curso->grupos[0].horarios,
                               MAX_HORARIOS_POR_GRUPO);
    if (cant < 0 || cant == 0) return -7;
    curso->grupos[0].cantidadHorarios = cant;

    /* requisitos y correquisitos */
    if (!parsearCodigos(campos[9], &curso->requisitos)) {
        return -10;
    }
    if (!parsearCodigos(campos[10], &curso->correquisitos)) {
        return -11;
    }

    curso->cantidadGrupos = 1;
    return 0;
}

/* Busca un curso por su código dentro del arreglo. Devuelve su índice,
 * o -1 si no existe. */
static int buscarCurso(const Curso *cursos, int n, const char *codigo) {
    for (int i = 0; i < n; i++) {
        if (strcmp(cursos[i].codigo, codigo) == 0) return i;
    }
    return -1;
}

/*
 * Compara dos listas de códigos SIN importar el orden: son equivalentes si
 * contienen exactamente los mismos códigos, en cualquier posición.
 *
 * Recibe: dos listas de Requisitos.
 * Devuelve: 1 si tienen los mismos códigos, 0 si no.
 */
static int mismosCodigos(const Requisitos *a, const Requisitos *b) {
    if (a->cantidad != b->cantidad) return 0;

    int usados[MAX_REQUISITOS] = {0};
    for (int i = 0; i < a->cantidad; i++) {
        int encontrado = 0;
        for (int j = 0; j < b->cantidad; j++) {
            if (!usados[j] && strcmp(a->codigos[i], b->codigos[j]) == 0) {
                usados[j] = 1;
                encontrado = 1;
                break;
            }
        }
        if (!encontrado) return 0;
    }
    return 1;
}

/*
 * Compara dos grupos completos (número, tipo, profesor y sus horarios).
 * Los horarios se comparan SIN importar el orden: son equivalentes si contienen
 * exactamente los mismos horarios, en cualquier posición.
 *
 * Recibe: dos grupos.
 * Devuelve: 1 si son equivalentes, 0 si difieren en algo.
 */
static int mismoGrupoEquivalente(const Grupo *a, const Grupo *b) {
    if (a->numero != b->numero) return 0;
    if (a->cantidadHorarios != b->cantidadHorarios) return 0;
    if (strcmp(a->tipo, b->tipo) != 0) return 0;
    if (strcmp(a->profesor, b->profesor) != 0) return 0;

    int usados[MAX_HORARIOS_POR_GRUPO] = {0};
    for (int i = 0; i < a->cantidadHorarios; i++) {
        int encontrado = 0;
        for (int j = 0; j < b->cantidadHorarios; j++) {
            if (!usados[j] && mismoHorario(&a->horarios[i], &b->horarios[j])) {
                usados[j] = 1;
                encontrado = 1;
                break;
            }
        }
        if (!encontrado) return 0;
    }
    return 1;
}

/*
 * Verifica si `nuevo` es un registro exactamente duplicado de alguno que ya
 * esté cargado dentro de `existente`. Un duplicado es exactamente el mismo
 * registro: mismo código, nombre, créditos, horas, requisitos/correquisitos
 * (en cualquier orden) y el mismo grupo (número, tipo, profesor y horarios en
 * cualquier orden).
 *
 * Devuelve: 1 si es duplicado exacto, 0 si no.
 */
static int registroDuplicado(const Curso *existente, const Curso *nuevo) {
    /* El código ya se igualó al buscar el curso en el catálogo. */
    if (strcmp(existente->nombre, nuevo->nombre) != 0) return 0;
    if (existente->creditos != nuevo->creditos ||
        existente->horas != nuevo->horas) {
        return 0;
    }
    if (!mismosCodigos(&existente->requisitos, &nuevo->requisitos)) return 0;
    if (!mismosCodigos(&existente->correquisitos, &nuevo->correquisitos)) {
        return 0;
    }
    for (int i = 0; i < existente->cantidadGrupos; i++) {
        if (mismoGrupoEquivalente(&existente->grupos[i], &nuevo->grupos[0])) {
            return 1;
        }
    }
    return 0;
}

/* Devuelve: 1 si el curso ya tiene un grupo con el número indicado.
 * Sirve para detectar números de grupo repetidos como posible inconsistencia
 * (por ejemplo, dos registros con "grupo 2" aunque el resto difiera). */
static int cursoTieneGrupoNumero(const Curso *curso, int numero) {
    for (int i = 0; i < curso->cantidadGrupos; i++) {
        if (curso->grupos[i].numero == numero) return 1;
    }
    return 0;
}

/* Revisa que los datos básicos del curso coincidan entre registros de la
 * misma materia y avisa (con una advertencia) si no lo hacen. */
static void verificarCoherencia(const Curso *existente, const Curso *nuevo,
                                int numLinea) {
    if (strcmp(existente->nombre, nuevo->nombre) != 0) {
        fprintf(stderr,
                "  [Línea %d] Advertencia: el curso %s tiene registros con "
                "nombres distintos ('%s' vs '%s').\n",
                numLinea, existente->codigo, existente->nombre, nuevo->nombre);
    }
    if (existente->creditos != nuevo->creditos ||
        existente->horas != nuevo->horas) {
        fprintf(stderr,
                "  [Línea %d] Advertencia: el curso %s tiene créditos/horas "
                "inconsistentes entre sus registros.\n",
                numLinea, existente->codigo);
    }
}

/*
 * Verifica que cada requisito y correquisito de los cursos cargados apunte a
 * un curso que realmente exista en el catálogo. Solo reporta el problema, no
 * elimina nada.
 *
 * Recibe: el arreglo de cursos cargado y su cantidad.
 * Devuelve: la cantidad de referencias a cursos inexistentes (0 si todas ok).
 */
static int verificarRequisitosCatalogo(const Curso *cursos, int nCursos) {
    int faltantes = 0;

    for (int i = 0; i < nCursos; i++) {
        const Curso *c = &cursos[i];

        for (int r = 0; r < c->requisitos.cantidad; r++) {
            if (buscarCurso(cursos, nCursos, c->requisitos.codigos[r]) < 0) {
                faltantes++;
                fprintf(stderr,
                        "[Advertencia] El curso %s tiene el requisito %s, "
                        "pero ese curso no existe en el catálogo.\n",
                        c->codigo, c->requisitos.codigos[r]);
            }
        }
        for (int r = 0; r < c->correquisitos.cantidad; r++) {
            if (buscarCurso(cursos, nCursos, c->correquisitos.codigos[r]) < 0) {
                faltantes++;
                fprintf(stderr,
                        "[Advertencia] El curso %s tiene el correquisito %s, "
                        "pero ese curso no existe en el catálogo.\n",
                        c->codigo, c->correquisitos.codigos[r]);
            }
        }
    }

    if (faltantes > 0) {
        fprintf(stderr,
                "Validación de requisitos: %d referencia(s) a cursos "
                "inexistentes.\n",
                faltantes);
    } else {
        printf("Validación de requisitos: todos los requisitos y "
               "correquisitos existen en el catálogo.\n");
    }
    return faltantes;
}

/*
 * Abre data/cursos.csv y carga todos los cursos en memoria. Cada línea es un
 * grupo de un curso; las líneas con el mismo código se agrupan en un solo
 * Curso. Los registros exactamente duplicados se detectan y se omiten;
 * los números de grupo repetidos y los requisitos inexistentes se reportan
 * como advertencias sin eliminar datos.
 *
 * Recibe: arreglo de Curso donde cargar y su capacidad máxima.
 * Devuelve: la cantidad de cursos cargados, o -1 si no se pudo abrir el
 * archivo o los parámetros son inválidos.
 */
int cargarCursos(Curso *cursos, int maxCursos) {
    if (cursos == NULL || maxCursos <= 0) return -1;

    FILE *archivo = fopen(RUTA_CURSOS, "r");
    if (archivo == NULL) {
        fprintf(stderr, "[Error] No se pudo abrir el archivo %s.\n",
                RUTA_CURSOS);
        return -1;
    }

    char linea[TAM_LINEA];
    int numLinea = 0;
    int nCursos = 0;
    int nGrupos = 0;
    int nDuplicados = 0;
    int nErrores = 0;
    int esEncabezado = 1;

    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        numLinea++;
        recortar(linea);
        if (linea[0] == '\0') continue;

        if (esEncabezado) {
            esEncabezado = 0;
            if (strncmp(linea, "codigo", 6) == 0) continue;
        }

        Curso temp;
        int res = leerCurso(linea, &temp);
        if (res != 0) {
            nErrores++;
            fprintf(stderr, "[Error en línea %d] %s\n", numLinea,
                    textoErrorLeerCurso(res));
            fprintf(stderr, "  Registro: %s\n", linea);
            continue;
        }

        int idx = buscarCurso(cursos, nCursos, temp.codigo);

        if (idx < 0) {
            if (nCursos >= maxCursos) {
                nErrores++;
                fprintf(stderr,
                        "[Error en línea %d] Se alcanzó el máximo de %d "
                        "cursos; se omite %s.\n",
                        numLinea, maxCursos, temp.codigo);
                continue;
            }
            cursos[nCursos] = temp;
            nCursos++;
            nGrupos++;
            continue;
        }

        /* El curso ya existía: se agrega el nuevo grupo. */
        verificarCoherencia(&cursos[idx], &temp, numLinea);

        if (registroDuplicado(&cursos[idx], &temp)) {
            nDuplicados++;
            fprintf(stderr,
                    "[Línea %d] Registro duplicado: %s (grupo %d, %s, %s). "
                    "Se omite para no cargarlo dos veces.\n",
                    numLinea, temp.codigo, temp.grupos[0].numero,
                    temp.grupos[0].tipo, temp.grupos[0].profesor);
            continue;
        }

        if (cursoTieneGrupoNumero(&cursos[idx], temp.grupos[0].numero)) {
            fprintf(stderr,
                    "[Línea %d] Advertencia: el curso %s ya tiene un grupo "
                    "con el número %d; es un posible registro repetido. Se "
                    "conserva por si representa una situación real de la "
                    "Guía de Horarios.\n",
                    numLinea, cursos[idx].codigo, temp.grupos[0].numero);
        }

        if (cursos[idx].cantidadGrupos >= MAX_GRUPOS_POR_CURSO) {
            nErrores++;
            fprintf(stderr,
                    "[Error en línea %d] Exceso de grupos para %s (máximo %d). "
                    "Se omite.\n",
                    numLinea, temp.codigo, MAX_GRUPOS_POR_CURSO);
            continue;
        }

        cursos[idx].grupos[cursos[idx].cantidadGrupos] = temp.grupos[0];
        cursos[idx].cantidadGrupos++;
        nGrupos++;
    }

    fclose(archivo);

    /* Verificación posterior a la carga: requisitos/correquisitos deben
     * apuntar a cursos que existen en el catálogo. No se hace dentro de
     * leerCurso() porque al leer una línea aún no se conocen todos los
     * cursos. */
    verificarRequisitosCatalogo(cursos, nCursos);

    printf("\n=== Resumen de carga ===\n");
    printf("Cursos cargados    : %d\n", nCursos);
    printf("Grupos cargados    : %d\n", nGrupos);
    printf("Duplicados omitidos: %d\n", nDuplicados);
    printf("Líneas con errores : %d\n", nErrores);

    return nCursos;
}
