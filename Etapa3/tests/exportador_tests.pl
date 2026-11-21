% ========================================================
% PRUEBAS: Exportación de Resultados
% Autor: Persona 3 (Control de Flujo y Exportación)
% Descripción: Exporta un catálogo de ejemplo evaluado con
%              las reglas reales y verifica el archivo
%              resultante línea por línea.
% ========================================================

:- consult('../src/exportador.pl').

recorrido:combinacion(c1,
    [curso('CE1101', 3, 3, 'Programacion', 'Ingenieria en Computadores'),
     curso('EM5001', 1, 4, 'Matematica', 'Ensenanza de la Matematica')]).

recorrido:combinacion(c2,
    [curso('CE1101', 3, 3, 'Programacion', 'Ingenieria en Computadores'),
     curso('CE1103', 2, 4, 'Programacion', 'Ingenieria en Computadores')]).

recorrido:combinacion(c3,
    [curso('CE1105', 1, 3, 'Programacion', 'Ingenieria en Computadores'),
     curso('CE2103', 1, 3, 'Programacion', 'Ingenieria en Computadores'),
     curso('EM5002', 2, 4, 'Programacion', 'Ensenanza de la Matematica')]).

recorrido:combinacion(c4,
    [curso('CE1105', 1, 4, 'Programacion', 'Ingenieria en Computadores'),
     curso('MA1103', 2, 4, 'Matematica', 'Ingenieria en Computadores')]).

:- begin_tests(exportador).

test(exporta_la_cantidad_correcta) :-
    tmp_file(resultados, Ruta),
    exportar_resultados(Ruta, Total),
    Total == 4.

test(archivo_linea_por_linea) :-
    tmp_file(resultados, Ruta),
    exportar_resultados(Ruta, _),
    read_file_to_string(Ruta, Texto, []),
    split_string(Texto, "\n", "\r", Lineas),
    exclude(==(""), Lineas, LineasUtiles),
    LineasUtiles ==
        ["combinacion;cursos;veredicto;razon",
         "c1;CE1101-G3|EM5001-G1;cumple;Ninguna",
         "c2;CE1101-G3|CE1103-G2;no_cumple;Violacion de regla de incompatibilidad",
         "c3;CE1105-G1|CE2103-G1|EM5002-G2;no_cumple;Exceso de cursos permitidos por area",
         "c4;CE1105-G1|MA1103-G2;no_cumple;Incumplimiento de balance de carreras"].

:- end_tests(exportador).
