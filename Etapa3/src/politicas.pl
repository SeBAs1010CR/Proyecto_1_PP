% ========================================================
% MÓDULO: Políticas Académicas (politicas.pl)
% Autor: Persona 2 (Motor de Reglas)
% Descripción: Valida las reglas de política académica 
%              para cada combinación de cursos generada.
% ========================================================

:- module(politicas, [evaluar_combinacion/3]).

% --------------------------------------------------------
% 1. HECHOS DE POLÍTICA (Parámetros configurables)
% --------------------------------------------------------
% Porcentaje máximo de créditos permitidos por carrera en una combinación.
max_porcentaje_carrera('Ingenieria en Computadores', 70).

% Límite máximo de cursos permitidos por área específica.
limite_area('Programacion', 2).
limite_area('Matematica', 2).
limite_area('Fisica', 1).

% Pares de cursos incompatibles (Definición base y simétrica).
incompatible_base('CE1101', 'CE1103').
incompatible_base('MA1403', 'FI1101').

incompatible(C1, C2) :- incompatible_base(C1, C2).
incompatible(C1, C2) :- incompatible_base(C2, C1).


% --------------------------------------------------------
% 2. REGLA 3: INCOMPATIBILIDAD
% --------------------------------------------------------
% Predicado: cumple_incompatibilidad(+Combinacion)
% Valida que la combinación NO incluya simultáneamente ambos cursos 
% de un par incompatible. Utiliza negación por falla (\+).
cumple_incompatibilidad(Combinacion) :-
    \+ (
        incompatible(C1, C2),
        member(curso(C1, _, _, _, _), Combinacion),
        member(curso(C2, _, _, _, _), Combinacion)
    ).


% ========================================================
% 3. REGLA 2: LÍMITE POR ÁREA
% ========================================================

% Auxiliar: Verifica si un término curso pertenece al área buscada.
curso_de_area(AreaBuscada, curso(_, _, _, AreaCurso, _)) :-
    AreaCurso == AreaBuscada.

% Predicado: contar_cursos_area(+AreaBuscada, +Combinacion, -Conteo)
% Cuenta de forma eficiente y idiomática los cursos de un área en la combinación
% utilizando filtrado funcional de listas (`include`).
contar_cursos_area(AreaBuscada, Combinacion, Conteo) :-
    include(curso_de_area(AreaBuscada), Combinacion, CursosFiltrados),
    length(CursosFiltrados, Conteo).

% Predicado: cumple_limite_area(+Area, +Combinacion)
% Comprueba que la cantidad de cursos de un área no rebase el límite permitido.
cumple_limite_area(Area, Combinacion) :-
    limite_area(Area, MaxPermitido),
    contar_cursos_area(Area, Combinacion, Conteo),
    Conteo =< MaxPermitido.

% Predicado: cumple_limite_area_total(+Combinacion)
% Verifica el cumplimiento de los límites para todas las áreas definidas en los hechos.
cumple_limite_area_total(Combinacion) :-
    findall(Area, limite_area(Area, _), Areas),
    verificar_todas_areas(Areas, Combinacion).

verificar_todas_areas([], _).
verificar_todas_areas([Area | Resto], Combinacion) :-
    cumple_limite_area(Area, Combinacion),
    verificar_todas_areas(Resto, Combinacion).


% ========================================================
% 4. REGLA 1: BALANCE ENTRE CARRERAS
% ========================================================

% Predicado: suma_creditos(+Combinacion, -Total)
% Suma recursivamente los créditos de todos los cursos presentes en la combinación.
suma_creditos([], 0).
suma_creditos([curso(_, _, Creditos, _, _) | Resto], Total) :-
    suma_creditos(Resto, SubTotal),
    Total is SubTotal + Creditos.

% Predicado: suma_creditos_carrera(+CarreraBuscada, +Combinacion, -Total)
% Suma los créditos específicos acumulados por una carrera particular en la combinación.
suma_creditos_carrera(_, [], 0).
suma_creditos_carrera(CarreraBuscada, [curso(_, _, Creditos, _, Carrera) | Resto], Total) :-
    (   Carrera == CarreraBuscada
    ->  suma_creditos_carrera(CarreraBuscada, Resto, SubTotal),
        Total is SubTotal + Creditos
    ;   suma_creditos_carrera(CarreraBuscada, Resto, Total)
    ).

% Predicado: cumple_balance_carreras(+Combinacion)
% Valida que ninguna carrera rebase su porcentaje máximo de créditos permitidos.
cumple_balance_carreras(Combinacion) :-
    suma_creditos(Combinacion, TotalCreditos),
    TotalCreditos > 0,
    % Extrae de forma única todas las carreras presentes en la combinación
    findall(C, member(curso(_, _, _, _, C), Combinacion), CarrerasRepetidas),
    list_to_set(CarrerasRepetidas, CarrerasUnicas),
    validar_porcentajes_carreras(CarrerasUnicas, Combinacion, TotalCreditos).

validar_porcentajes_carreras([], _, _).
validar_porcentajes_carreras([Carrera | Resto], Combinacion, TotalCreditos) :-
    (   max_porcentaje_carrera(Carrera, MaxPorcentaje)
    ->  true
    ;   MaxPorcentaje = 100 % Si no está configurada, se asume 100% por defecto
    ),
    suma_creditos_carrera(Carrera, Combinacion, CreditosCarrera),
    Porcentaje is (CreditosCarrera / TotalCreditos) * 100,
    Porcentaje =< MaxPorcentaje,
    validar_porcentajes_carreras(Resto, Combinacion, TotalCreditos).


% ========================================================
% 5. EVALUADOR PRINCIPAL Y RAZÓN DE RECHAZO (Cortes y Negación)
% ========================================================
% Predicado: evaluar_combinacion(+Combinacion, -Veredicto, -Razon)
% Controla el flujo de validación institucional. El orden de las cláusulas 
% y el uso de cortes (!) aseguran que se capture y reporte únicamente 
% la primera violación encontrada.

evaluar_combinacion(Combinacion, cumple, 'Ninguna') :-
    cumple_incompatibilidad(Combinacion),
    cumple_limite_area_total(Combinacion),
    cumple_balance_carreras(Combinacion),
    !.

evaluar_combinacion(Combinacion, no_cumple, 'Violacion de regla de incompatibilidad') :-
    \+ cumple_incompatibilidad(Combinacion), !.

evaluar_combinacion(Combinacion, no_cumple, 'Exceso de cursos permitidos por area') :-
    \+ cumple_limite_area_total(Combinacion), !.

evaluar_combinacion(Combinacion, no_cumple, 'Incumplimiento de balance de carreras') :-
    \+ cumple_balance_carreras(Combinacion).