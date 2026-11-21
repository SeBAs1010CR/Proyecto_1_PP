% Pruebas del evaluador. Usa reglas simuladas: la combinación se representa
% como una lista de marcas de violación, así cada prueba controla qué
% reglas fallan. Las reglas reales las define el motor de reglas.

:- consult('../src/evaluador.pl').

cumple_regla(balance_carreras, Combinacion) :-
    \+ member(viola_balance, Combinacion).
cumple_regla(limite_por_area, Combinacion) :-
    \+ member(viola_area, Combinacion).
cumple_regla(incompatibilidad, Combinacion) :-
    \+ member(viola_incompatibles, Combinacion).

:- begin_tests(evaluador).

test(sin_violaciones_cumple) :-
    evaluar([], cumple).

test(una_regla_violada) :-
    evaluar([viola_area], rechazada(limite_por_area)).

test(devuelve_la_primera_segun_el_orden) :-
    evaluar([viola_incompatibles, viola_balance], rechazada(balance_carreras)).

test(todas_violadas_reporta_la_primera) :-
    evaluar([viola_balance, viola_area, viola_incompatibles],
            rechazada(balance_carreras)).

test(ultima_regla_del_orden) :-
    evaluar([viola_incompatibles], rechazada(incompatibilidad)).

test(veredicto_unico_sin_backtracking) :-
    findall(R, evaluar([viola_area, viola_incompatibles], R),
            [rechazada(limite_por_area)]).

test(cumple_tambien_es_unico) :-
    findall(R, evaluar([], R), [cumple]).

:- end_tests(evaluador).
