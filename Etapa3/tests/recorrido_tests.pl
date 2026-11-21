% ========================================================
% PRUEBAS: Recorrido de Combinaciones
% Autor: Persona 3 (Control de Flujo y Exportación)
% Descripción: Evalúa combinaciones de ejemplo contra las
%              reglas reales de politicas.pl, cubriendo un
%              caso por regla y los modos de consulta.
% ========================================================

:- consult('../src/recorrido.pl').

% c1 cumple: áreas dentro del límite, carreras balanceadas, sin incompatibles.
recorrido:combinacion(c1,
    [curso('CE1101', 'Introduccion a la programacion', 3, 'Programacion', 'Ingenieria en Computadores'),
     curso('EM5001', 'Matematica para la ensenanza', 4, 'Matematica', 'Ensenanza de la Matematica')]).

% c2 viola incompatibilidad: CE1101 y CE1103 son un par incompatible.
recorrido:combinacion(c2,
    [curso('CE1101', 'Introduccion a la programacion', 3, 'Programacion', 'Ingenieria en Computadores'),
     curso('CE1103', 'Estructuras de datos', 4, 'Programacion', 'Ingenieria en Computadores')]).

% c3 viola límite por área: tres cursos de Programacion (máximo 2).
recorrido:combinacion(c3,
    [curso('CE1105', 'Programacion I', 3, 'Programacion', 'Ingenieria en Computadores'),
     curso('CE2103', 'Programacion II', 3, 'Programacion', 'Ingenieria en Computadores'),
     curso('EM5002', 'Algoritmos educativos', 4, 'Programacion', 'Ensenanza de la Matematica')]).

% c4 viola balance: 100% de los créditos son de Ingenieria en Computadores (máximo 70%).
recorrido:combinacion(c4,
    [curso('CE1105', 'Programacion I', 4, 'Programacion', 'Ingenieria en Computadores'),
     curso('MA1103', 'Calculo', 4, 'Matematica', 'Ingenieria en Computadores')]).

:- begin_tests(recorrido).

test(evalua_todas_las_combinaciones) :-
    evaluar_catalogo(Resultados),
    length(Resultados, 4).

test(combinacion_que_cumple) :-
    evaluar_catalogo(Resultados),
    memberchk(resultado(c1, cumple, 'Ninguna'), Resultados).

test(rechazo_por_incompatibilidad) :-
    evaluar_catalogo(Resultados),
    memberchk(resultado(c2, no_cumple, 'Violacion de regla de incompatibilidad'),
              Resultados).

test(rechazo_por_limite_de_area) :-
    evaluar_catalogo(Resultados),
    memberchk(resultado(c3, no_cumple, 'Exceso de cursos permitidos por area'),
              Resultados).

test(rechazo_por_balance_de_carreras) :-
    evaluar_catalogo(Resultados),
    memberchk(resultado(c4, no_cumple, 'Incumplimiento de balance de carreras'),
              Resultados).

% Modos de consulta adicionales del predicado reversible.
test(modo_solo_las_que_cumplen) :-
    findall(Id, combinacion_evaluada(Id, cumple, _), [c1]).

test(modo_buscar_por_razon) :-
    findall(Id,
            combinacion_evaluada(Id, _, 'Incumplimiento de balance de carreras'),
            [c4]).

test(modo_consulta_puntual) :-
    combinacion_evaluada(c2, no_cumple, _).

:- end_tests(recorrido).
