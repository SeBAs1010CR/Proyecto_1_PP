% carga.pl
%
% Módulo de carga de datos de la Etapa 3 (Prolog), Persona 1.
%
% Lee tres fuentes y las deja como hechos en la base de conocimiento:
%   1. El catálogo que exporta C (output/catalogo_salida.csv)
%      -> curso(Codigo, Creditos, Carrera, Area), un hecho por curso.
%   2. Las combinaciones que genera Racket, una por línea, con códigos
%      separados por coma (ej.: CE1101,MA1102,SE1201)
%      -> combinacion(Id, [Codigo, ...]), Id empieza en 1.
%   3. Las políticas académicas (hechos incompatible/2 de data/politicas.pl)
%      -> incompatible(CodigoA, CodigoB).
%
% Este módulo NO filtra por créditos ni genera combinaciones: solo carga.

:- dynamic curso/4.
:- dynamic combinacion/2.
:- dynamic incompatible/2.

% cargar_catalogo(+Ruta)
%
% Carga el catálogo de C. Se ignora el encabezado y, si un código aparece
% en varias líneas (un registro por grupo), solo se guarda la primera vez.
cargar_catalogo(Ruta) :-
    retractall(curso(_, _, _, _)),
    leer_lineas(Ruta, [_Encabezado | Datos]),
    maplist(registrar_curso, Datos).

registrar_curso(Linea) :-
    split_string(Linea, ";", "", Campos),
    (   Campos = [Codigo, _Nombre, CreditosS, _Grupo, _Tipo, _Profesor,
                  _Horarios, _Requisitos, _Correquisitos, _Choca,
                  _PuedeMatricular, Carrera, Area]
    ->  number_string(Creditos, CreditosS),
        atom_string(CodigoA, Codigo),
        (   curso(CodigoA, _, _, _)
        ->  true
        ;   atom_string(CarreraA, Carrera),
            atom_string(AreaA, Area),
            assertz(curso(CodigoA, Creditos, CarreraA, AreaA))
        )
    ;   throw(error(formato_catalogo(Linea), _))
    ).

% cargar_combinaciones(+Ruta)
%
% Carga las combinaciones generadas por Racket. Cada línea no vacía es una
% combinación. Los Id se numeran en orden de aparición desde 1.
cargar_combinaciones(Ruta) :-
    retractall(combinacion(_, _)),
    leer_lineas(Ruta, Lineas),
    foldl(registrar_combinacion, Lineas, 1, _).

registrar_combinacion(Linea, Id, Siguiente) :-
    split_string(Linea, ",", " ", CodigosS),
    maplist([S, A]>>atom_string(A, S), CodigosS, Codigos),
    assertz(combinacion(Id, Codigos)),
    Siguiente is Id + 1.

% cargar_politicas(+Ruta)
%
% Lee los hechos de políticas (incompatible/2) de un archivo Prolog.
% Solo se aceptan hechos incompatible/2; cualquier otro término se ignora.
cargar_politicas(Ruta) :-
    retractall(incompatible(_, _)),
    setup_call_cleanup(
        open(Ruta, read, Flujo, [encoding(utf8)]),
        leer_politicas(Flujo),
        close(Flujo)).

leer_politicas(Flujo) :-
    read_term(Flujo, Termino, []),
    (   Termino == end_of_file
    ->  true
    ;   (   Termino = incompatible(_, _)
        ->  assertz(Termino)
        ;   true
        ),
        leer_politicas(Flujo)
    ).

% leer_lineas(+Ruta, -Lineas)
%
% Devuelve las líneas no vacías del archivo como strings, sin espacios ni
% retornos de carro en los extremos. Usa UTF-8 para los acentos del catálogo.
leer_lineas(Ruta, Lineas) :-
    read_file_to_string(Ruta, Texto, [encoding(utf8)]),
    split_string(Texto, "\n", "\r ", Crudas),
    exclude(==(""), Crudas, Lineas).
