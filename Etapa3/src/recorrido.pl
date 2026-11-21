% ========================================================
% MÓDULO: Recorrido de Combinaciones (recorrido.pl)
% Autor: Persona 3 (Control de Flujo y Exportación)
% Descripción: Evalúa por backtracking todas las
%              combinaciones cargadas contra el motor de
%              reglas de política académica.
% ========================================================

:- module(recorrido, [combinacion_evaluada/3, evaluar_catalogo/1]).

:- use_module(politicas).

% Contrato con el módulo de carga (Persona 1): cada combinación leída del
% archivo de Racket se registra como un hecho
%   recorrido:combinacion(Id, Cursos)
% donde Cursos es una lista de curso(Codigo, Nombre, Creditos, Area, Carrera).
:- multifile combinacion/2.
:- dynamic combinacion/2.

% --------------------------------------------------------
% 1. EVALUACIÓN POR BACKTRACKING
% --------------------------------------------------------
% Predicado: combinacion_evaluada(?Id, ?Veredicto, ?Razon)
% Punto de backtracking del módulo: por re-satisfacción enumera una a una
% las combinaciones cargadas con su veredicto y su razón de rechazo.
% Es reversible, por lo que admite varios modos de consulta:
%   combinacion_evaluada(Id, V, R)         → recorre todas
%   combinacion_evaluada(c1, V, R)         → evalúa una combinación puntual
%   combinacion_evaluada(Id, cumple, _)    → solo las que cumplen
%   combinacion_evaluada(Id, _, Razon)     → las rechazadas por una razón dada
%
% El motor evaluar_combinacion/3 usa cortes en el cuerpo de sus cláusulas,
% por lo que solo es correcto con veredicto y razón sin instanciar. Para que
% este predicado sí sea reversible, se evalúa primero con variables frescas
% (el modo natural del motor) y la unificación con los argumentos del
% consultante se hace después.
combinacion_evaluada(Id, Veredicto, Razon) :-
    combinacion(Id, Cursos),
    evaluar_combinacion(Cursos, VeredictoReal, RazonReal),
    Veredicto = VeredictoReal,
    Razon = RazonReal.

% --------------------------------------------------------
% 2. RECORRIDO EXHAUSTIVO
% --------------------------------------------------------
% Predicado: evaluar_catalogo(-Resultados)
% findall fuerza el backtracking exhaustivo sobre combinacion_evaluada/3 y
% reúne un resultado(Id, Veredicto, Razon) por cada combinación cargada.
evaluar_catalogo(Resultados) :-
    findall(resultado(Id, Veredicto, Razon),
            combinacion_evaluada(Id, Veredicto, Razon),
            Resultados).
