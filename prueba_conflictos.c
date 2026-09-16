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