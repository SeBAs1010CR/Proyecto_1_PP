% evaluador.pl
%
% Determina el veredicto de una combinación contra las reglas de política
% académica. Si la combinación viola alguna regla, devuelve exactamente la
% primera regla violada según el orden oficial de evaluación.
%
% El contrato con el motor de reglas es cumple_regla(Regla, Combinacion).
% Se declara dynamic para que este archivo cargue aunque las reglas reales
% todavía no estén definidas.

:- dynamic cumple_regla/2.

% Orden oficial de evaluación de las reglas de política.
orden_reglas([balance_carreras, limite_por_area, incompatibilidad]).

% evaluar(+Combinacion, -Resultado)
% Resultado es cumple o rechazada(Regla). El corte garantiza un veredicto
% único: si hay una regla violada, no se intenta el veredicto cumple.
evaluar(Combinacion, rechazada(Regla)) :-
    orden_reglas(Reglas),
    primera_regla_violada(Reglas, Combinacion, Regla),
    !.
evaluar(_, cumple).

% primera_regla_violada(+Reglas, +Combinacion, -Regla)
% Recorre las reglas en orden. La negación por falla (\+) detecta la
% violación y el corte (!) evita que el backtracking devuelva también
% las reglas violadas que vienen después de la primera.
primera_regla_violada([Regla|_], Combinacion, Regla) :-
    \+ cumple_regla(Regla, Combinacion),
    !.
primera_regla_violada([_|Resto], Combinacion, Regla) :-
    primera_regla_violada(Resto, Combinacion, Regla).
