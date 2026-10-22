/*
 * prueba_conflictos.c
 * 
 * Programa de prueba (Persona 2 - Ian Alejandro Bonilla Mena)
 * 
 * Propósito: Verificar el caso límite de la detección de choques:
 * dos cursos consecutivos (8:00-10:00 y 10:00-12:00) NO deben considerarse
 * choque porque no se superponen realmente.
 * 
 * Nota: este archivo contiene la única función main() del repositorio;
 * sirve como prueba aislada del módulo de conflictos.
 * 
 * Compilación: gcc -Iinclude prueba_conflictos.c src/conflictos.c src/horario.c \
 *              src/grupo.c src/curso.c src/requisitos.c -o prueba_conflictos.exe
 */

#include <stdio.h>
#include <string.h>
#include "conflictos.h"

int main() {
    Horario h1, h2;
    
    // Simulamos el caso límite del PDF
    strcpy(h1.dia, "Lunes");
    h1.horaInicio = 800;
    h1.horaFin = 1000;

    strcpy(h2.dia, "Lunes");
    h2.horaInicio = 1000;
    h2.horaFin = 1200;

    printf("Prueba del caso limite (8-10 y 10-12):\n");
    if (horariosChocan(h1, h2)) {
        printf("ERROR: El sistema dice que CHOCAN.\n");
    } else {
        printf("EXITO: El sistema dice que NO CHOCAN.\n");
    }

    return 0;
}