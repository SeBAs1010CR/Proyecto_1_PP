% ========================================================
% MÓDULO: Evaluador y Razón de Rechazo (evaluador.pl)
% Autor: Persona 3 (Control de Flujo y Exportación)
% Descripción: Determina el veredicto de una combinación y,
%              si falla, devuelve exactamente la primera
%              regla violada según el orden oficial.
% ========================================================

:- module(evaluador, [evaluar/2, orden_reglas/1]).

% Contrato con el motor de reglas (politicas.pl): cada regla se registra
% como una cláusula evaluador:cumple_regla(Regla, Combinacion). Se declara
% multifile/dynamic para que este módulo cargue aunque las reglas reales
% todavía no estén conectadas.
:- multifile cumple_regla/2.
:- dynamic cumple_regla/2.

% --------------------------------------------------------
% 1. ORDEN OFICIAL DE EVALUACIÓN
% --------------------------------------------------------
% Mismo orden que aplica el motor de reglas de la Persona 2.
orden_reglas([incompatibilidad, limite_por_area, balance_carreras]).

% --------------------------------------------------------
% 2. VEREDICTO CON RAZÓN DE RECHAZO
% --------------------------------------------------------
% Predicado: evaluar(+Combinacion, -Resultado)
% Resultado es cumple o rechazada(Regla). El corte (!) garantiza un
% veredicto único: si existe una regla violada, nunca se llega al
% veredicto cumple ni se devuelve más de una razón.
evaluar(Combinacion, rechazada(Regla)) :-
    orden_reglas(Reglas),
    primera_regla_violada(Reglas, Combinacion, Regla),
    !.
evaluar(_, cumple).

% Predicado: primera_regla_violada(+Reglas, +Combinacion, -Regla)
% Recorre las reglas en el orden oficial. La negación por falla (\+)
% detecta la violación y el corte evita que el backtracking devuelva
% también las reglas violadas que vienen después de la primera.
primera_regla_violada([Regla|_], Combinacion, Regla) :-
    \+ cumple_regla(Regla, Combinacion),
    !.
primera_regla_violada([_|Resto], Combinacion, Regla) :-
    primera_regla_violada(Resto, Combinacion, Regla).
