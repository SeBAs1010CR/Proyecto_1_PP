% ========================================================
% PRUEBAS: Evaluador y Razón de Rechazo
% Autor: Persona 3 (Control de Flujo y Exportación)
% Descripción: Pruebas con reglas simuladas. La combinación
%              se representa como una lista de marcas de
%              violación, así cada prueba controla qué
%              reglas fallan sin depender del motor real.
% ========================================================

:- consult('../src/evaluador.pl').

evaluador:cumple_regla(incompatibilidad, Combinacion) :-
    \+ member(viola_incompatibles, Combinacion).
evaluador:cumple_regla(limite_por_area, Combinacion) :-
    \+ member(viola_area, Combinacion).
evaluador:cumple_regla(balance_carreras, Combinacion) :-
    \+ member(viola_balance, Combinacion).

:- begin_tests(evaluador).

test(sin_violaciones_cumple) :-
    evaluar([], cumple).

test(una_regla_violada) :-
    evaluar([viola_area], rechazada(limite_por_area)).

test(devuelve_la_primera_segun_el_orden) :-
    evaluar([viola_balance, viola_incompatibles], rechazada(incompatibilidad)).

test(todas_violadas_reporta_la_primera) :-
    evaluar([viola_balance, viola_area, viola_incompatibles],
            rechazada(incompatibilidad)).

test(ultima_regla_del_orden) :-
    evaluar([viola_balance], rechazada(balance_carreras)).

test(veredicto_unico_sin_backtracking) :-
    findall(R, evaluar([viola_area, viola_balance], R),
            [rechazada(limite_por_area)]).

test(cumple_tambien_es_unico) :-
    findall(R, evaluar([], R), [cumple]).

:- end_tests(evaluador).
