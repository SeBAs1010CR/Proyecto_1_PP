/*
 * horario.c
 *
 * Funciones para leer, parsear y validar horarios.
 *
 * Representación de la hora: entero en formato HHMM, por ejemplo:
 *   07:30 -> 730    09:20 -> 920    18:00 -> 1800    20:50 -> 2050
 *
 * Se soportan los dos formatos de horario que aparecen en data/cursos.csv:
 *   - "MIE[07:30-09:20], VIE[07:30-09:20]"   (día en mayúsculas + corchetes)
 *   - "Mié 18:00-20:50 / Vie 18:00-19:50"    (día con tilde + guion)
 * Se aceptan tanto el guion simple '-' como el guion largo Unicode U+2013
 * que ya utiliza el CSV.
 */

#include <stdio.h>
#include <string.h>

#include "horario.h"

static int esDigito(char c) { return c >= '0' && c <= '9'; }

/*
 * Indica si `dia` es uno de los días normalizados válidos (3 letras
 * mayúsculas, sin tildes). Todos los días que hay en data/cursos.csv se
 * reducen a este conjunto.
 */
static int esDiaValido(const char *dia) {
    if (dia == NULL) return 0;

    static const char *dias[] = {"LUN", "MAR", "MIE", "JUE",
                                 "VIE", "SAB", "DOM"};
    for (int i = 0; i < 7; i++) {
        if (strcmp(dia, dias[i]) == 0) return 1;
    }
    return 0;
}

/*
 * Indica si un valor HHMM representa una hora real: 00..23 horas y
 * 00..59 minutos. Rechaza valores como 1865 (18:65), pues los minutos
 * sobrepasan 59.
 */
static int horaValida(int h) {
    if (h < 0) return 0;
    int hh = h / 100;
    int mm = h % 100;
    return hh <= 23 && mm <= 59;
}

/*
 * Lee una hora con el formato EXACTO HH:MM en la posición actual del texto.
 * No busca la aparición de "HH:MM" en otro punto de la cadena: si aquí no
 * hay dos dígitos, ':', dos dígitos más, se rechaza.
 *
 * Recibe: puntero al puntero de texto y puntero donde almacenar el valor.
 * Devuelve: 1 si hay una hora válida, 0 en caso contrario.
 */
static int leerHora(const char **pp, int *valor) {
    const char *p = *pp;
    if (strlen(p) < 5) return 0;

    if (!esDigito(p[0]) || !esDigito(p[1]) || p[2] != ':' ||
        !esDigito(p[3]) || !esDigito(p[4])) {
        return 0;
    }

    int hh = (p[0] - '0') * 10 + (p[1] - '0');
    int mm = (p[3] - '0') * 10 + (p[4] - '0');
    if (hh > 23 || mm > 59) return 0;

    *valor = hh * 100 + mm;
    *pp = p + 5;
    return 1;
}

/*
 * Consume el separador entre la hora de inicio y la de fin, que puede ser:
 *   - el guion simple '-'  (por ejemplo en "07:30-09:20" o "18:00-20:50");
 *   - el guion largo Unicode U+2013 ("–", bytes 0xE2 0x80 0x93) que usan
 *     los registros de Matemática General en el CSV.
 *
 * Recibe: puntero al puntero de texto.
 * Devuelve: 1 si encontró un separador válido, 0 en caso contrario.
 */
static int consumirSeparador(const char **pp) {
    const char *p = *pp;
    if (p[0] == '-') {
        *pp = p + 1;
        return 1;
    }
    size_t restos = strlen(p);
    if (restos >= 3 && (unsigned char)p[0] == 0xE2 &&
        (unsigned char)p[1] == 0x80 && (unsigned char)p[2] == 0x93) {
        *pp = p + 3;
        return 1;
    }
    return 0;
}

/*
 * Normaliza el nombre de un día a 3 letras mayúsculas sin tildes.
 * Solo se aceptan las grafías exactas que aparecen en data/cursos.csv:
 *   "Lun"/"LUN" -> LUN, "Mar"/"MAR" -> MAR, "Mié"/"MIE" -> MIE,
 *   "Jue"/"JUE" -> JUE, "Vie"/"VIE" -> VIE, "Sáb"/"SAB" -> SAB,
 *   "Dom"/"DOM" -> DOM
 *
 * Cualquier otra grafía (por ejemplo "Lunes", "Mie" o "Sab") se rechaza
 * por ser un día inexistente o mal escrito.
 *
 * Recibe el texto crudo y un arreglo de salida de al menos 4 caracteres.
 * Devuelve: 1 si se obtuvo un día válido, 0 en caso contrario.
 */
static int normalizarDia(const char *raw, char out[4]) {
    if (raw == NULL) return 0;
    size_t largo = strlen(raw);
    if (largo < 3 || largo > 8) return 0; /* "Mié" y "Sáb" usan 4 bytes */

    static const char *aceptados[] = {
        "Lun", "LUN", "Mar", "MAR", "Mié", "MIE",
        "Jue", "JUE", "Vie", "VIE", "Sáb", "SAB",
        "Dom", "DOM"};
    static const char *normalizados[] = {
        "LUN", "LUN", "MAR", "MAR", "MIE", "MIE",
        "JUE", "JUE", "VIE", "VIE", "SAB", "SAB",
        "DOM", "DOM"};

    for (int i = 0; i < 14; i++) {
        if (largo == strlen(aceptados[i]) && strcmp(raw, aceptados[i]) == 0) {
            strcpy(out, normalizados[i]);
            return 1;
        }
    }
    return 0;
}

/*
 * Parsea un SOLO bloque de horario, por ejemplo:
 *   "MIE[07:30-09:20]"  o  "Mié 18:00-20:50"
 *
 * Verifica la estructura completa del bloque:
 *   - un día válido;
 *   - una hora HH:MM de inicio;
 *   - un separador '-' o guion largo;
 *   - una hora HH:MM de finalización;
 *   - nada inesperado después de la hora final (excepto un ']' de cierre).
 *
 * Recibe el texto del bloque y un puntero a Horario.
 * Devuelve: 1 si el horario es válido, 0 en caso contrario.
 */
int parsearHorario(const char *bloque, Horario *horario) {
    if (bloque == NULL || horario == NULL) return 0;

    const char *p = bloque;
    while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;

    /* Día: hasta un espacio, '[', ',' o '/'. */
    const char *inicio = p;
    while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n' &&
           *p != '[' && *p != ',' && *p != '/') {
        p++;
    }
    if (p == inicio) return 0;

    size_t len = (size_t)(p - inicio);
    if (len >= 16) return 0;
    char diaCrudo[16];
    memcpy(diaCrudo, inicio, len);
    diaCrudo[len] = '\0';

    char dia[4];
    if (!normalizarDia(diaCrudo, dia)) return 0;

    /* Tras el día: espacios opcionales y, en el formato "MIE[...]", el
     * corchete de apertura. Después debe venir la hora exacta. */
    const char *q = p;
    while (*q == ' ' || *q == '\t') q++;
    if (*q == '[') q++;

    int hInicio = 0, hFin = 0;
    if (!leerHora(&q, &hInicio)) return 0;
    if (!consumirSeparador(&q)) return 0;
    if (!leerHora(&q, &hFin)) return 0;
    if (hInicio >= hFin) return 0;

    /* Después de la hora final solo puede haber un ']' de cierre y/o
     * espacios; cualquier otro contenido invalida el bloque. */
    if (*q == ']') q++;
    while (*q == ' ' || *q == '\t' || *q == '\r' || *q == '\n') q++;
    if (*q != '\0') return 0;

    snprintf(horario->dia, MAX_DIA_LEN, "%s", dia);
    horario->horaInicio = hInicio;
    horario->horaFin = hFin;
    return 1;
}

/*
 * Parsea el campo completo de horario, que puede contener uno o varios
 * bloques separados por "," o por "/". Ejemplos:
 *   "MIE[07:30-09:20], VIE[07:30-09:20]"
 *   "Mié 18:00-20:50 / Vie 18:00-19:50"
 *
 * Los espacios alrededor de cada bloque se ignoran. Se rechazan bloqueos
 * vacíos, separadores consecutivos (",,", "//"), un separador inicial o
 * un separador final con el campo incompleto.
 *
 * Recibe el texto del campo, un arreglo de Horario y su capacidad máxima.
 * Devuelve el número de horarios cargados (>= 1), o un valor negativo:
 *   -1 si hay más bloques de los que caben en el arreglo,
 *   -2 si algún bloque está mal formado o es inválido.
 */
int parsearHorarios(const char *texto, Horario *horarios, int maxHorarios) {
    if (texto == NULL || horarios == NULL || maxHorarios <= 0) return -2;

    int n = 0;
    const char *p = texto;

    while (1) {
        while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
        if (*p == '\0') {
            /* Se llegó al final tras haber consumido un separador, o el
             * campo está vacío: en ambos casos falta un bloque válido. */
            return -2;
        }

        const char *fin = p;
        while (*fin && *fin != ',' && *fin != '/') fin++;
        if (fin == p) return -2; /* bloque vacío (separador inicial o doble) */

        size_t len = (size_t)(fin - p);
        if (len >= 128) return -2;

        char bloque[128];
        memcpy(bloque, p, len);
        bloque[len] = '\0';

        Horario tmp;
        if (!parsearHorario(bloque, &tmp)) return -2;
        if (n >= maxHorarios) return -1;
        horarios[n++] = tmp;

        if (*fin == '\0') break;
        p = fin + 1;
    }
    return n;
}

/*
 * Valida un horario ya construido.
 *
 * Comprueba:
 *   - puntero no NULL;
 *   - día no vacío y perteneciente a los días válidos (LUN..DOM);
 *   - hora de inicio y hora de fin con formato y minutos correctos
 *     (00..23 horas, 00..59 minutos);
 *   - hora de inicio menor que la de fin.
 *
 * Recibe un puntero a Horario.
 * Devuelve: 1 si es válido, 0 en caso contrario.
 */
int validarHorario(const Horario *horario) {
    if (horario == NULL) return 0;
    if (horario->dia[0] == '\0') return 0;
    if (!esDiaValido(horario->dia)) return 0;
    if (!horaValida(horario->horaInicio) || !horaValida(horario->horaFin)) {
        return 0;
    }
    if (horario->horaInicio >= horario->horaFin) return 0;
    return 1;
}
