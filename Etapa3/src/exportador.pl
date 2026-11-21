% ========================================================
% MÓDULO: Exportación de Resultados (exportador.pl)
% Autor: Persona 3 (Control de Flujo y Exportación)
% Descripción: Escribe el veredicto de cada combinación y
%              su razón de rechazo en el archivo CSV que
%              consumirá la etapa 4 (Java).
% ========================================================

:- module(exportador, [exportar_resultados/2]).

:- use_module(recorrido).

encabezado('combinacion;cursos;veredicto;razon').

% --------------------------------------------------------
% 1. EXPORTACIÓN
% --------------------------------------------------------
% Predicado: exportar_resultados(+Ruta, -Total)
% Evalúa el catálogo completo (recorrido por backtracking) y escribe una
% línea por combinación. Devuelve la cantidad de resultados exportados.
% setup_call_cleanup garantiza que el archivo se cierre aunque falle algo.
exportar_resultados(Ruta, Total) :-
    evaluar_catalogo(Resultados),
    setup_call_cleanup(
        open(Ruta, write, Puerto),
        escribir_archivo(Puerto, Resultados, Total),
        close(Puerto)).

escribir_archivo(Puerto, Resultados, Total) :-
    encabezado(Encabezado),
    writeln(Puerto, Encabezado),
    escribir_resultados(Puerto, Resultados, 0, Total).

% Recorre los resultados con recursión y un acumulador para el conteo.
escribir_resultados(_, [], Total, Total).
escribir_resultados(Puerto, [resultado(Id, Veredicto, Razon)|Resto],
                    Acumulado, Total) :-
    pares_combinacion(Id, Pares),
    atomic_list_concat(Pares, '|', Cursos),
    format(Puerto, '~w;~w;~w;~w~n', [Id, Cursos, Veredicto, Razon]),
    Siguiente is Acumulado + 1,
    escribir_resultados(Puerto, Resto, Siguiente, Total).

% --------------------------------------------------------
% 2. FORMATO DE LOS CURSOS
% --------------------------------------------------------
% Predicado: pares_combinacion(+Id, -Pares)
% Convierte los cursos de la combinación en pares curso-grupo con el
% mismo formato de la etapa 2 (ej.: 'CE1101-G1').
pares_combinacion(Id, Pares) :-
    recorrido:combinacion(Id, Cursos),
    findall(Par,
            ( member(curso(Codigo, Grupo, _, _, _), Cursos),
              format(atom(Par), '~w-G~w', [Codigo, Grupo]) ),
            Pares).
