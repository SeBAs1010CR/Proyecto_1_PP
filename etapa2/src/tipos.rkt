#lang racket

;; tipos.rkt
;;
;; Estructuras de datos de la Etapa 2 (Racket). Cada campo tiene un contrato
;; explícito (struct/contract), así un dato mal formado falla en el momento
;; en que se construye, con un mensaje que indica el campo incorrecto.
;;
;; Todas las estructuras son inmutables: no hay set! ni campos mutables.

(provide (struct-out Horario)
         (struct-out Grupo)
         (struct-out Preferencias)
         (struct-out Combinacion))

;; Bloque horario de un grupo. Ej.: MIE 07:30-09:20
(struct/contract Horario
  ([dia string?]
   [hora-inicio string?]
   [hora-fin string?])
  #:transparent)

;; Un grupo de un curso, tal como lo exporta el módulo de C.
;;  codigo             : código del curso (ej. "CE1103")
;;  nombre             : nombre del curso
;;  creditos           : créditos del curso
;;  grupo-num          : número de grupo
;;  tipo               : tipo de grupo (Regular, Semipresencial, ...)
;;  profesor           : nombre del profesor
;;  horarios           : lista de Horario
;;  requisitos         : códigos de requisitos
;;  correquisitos      : códigos de correquisitos
;;  choca              : 1 si el curso choca con otro del catálogo, 0 si no
;;  puede-matricular   : 1 si el estudiante cumple requisitos, 0 si no
;;  carrera            : carrera que aporta el curso
(struct/contract Grupo
  ([codigo string?]
   [nombre string?]
   [creditos exact-nonnegative-integer?]
   [grupo-num exact-nonnegative-integer?]
   [tipo string?]
   [profesor string?]
   [horarios (listof Horario?)]
   [requisitos (listof string?)]
   [correquisitos (listof string?)]
   [choca (or/c 0 1)]
   [puede-matricular (or/c 0 1)]
   [carrera string?])
  #:transparent)

;; Preferencias del estudiante.
;;  cursos-interes : lista de pares (codigo . numero-grupo)
;;  creditos-min   : créditos mínimos deseados
;;  creditos-max   : créditos máximos deseados
(struct/contract Preferencias
  ([cursos-interes (listof (cons/c string? exact-nonnegative-integer?))]
   [creditos-min exact-nonnegative-integer?]
   [creditos-max exact-nonnegative-integer?])
  #:transparent)

;; Una combinación de horario (la usará el Rol 2).
;;  grupos           : grupos elegidos, uno por curso
;;  creditos-totales : suma de créditos de los grupos
;;  carreras         : carreras de origen de los cursos incluidos
(struct/contract Combinacion
  ([grupos (listof Grupo?)]
   [creditos-totales exact-nonnegative-integer?]
   [carreras (listof string?)])
  #:transparent)
