#lang racket

;; Pruebas del módulo de salida.

(require rackunit
         racket/stream
         "../src/salida.rkt"
         "../src/tipos.rkt")

;; grupo mínimo válido para armar combinaciones de prueba
(define (grupo-de codigo num creditos carrera)
  (Grupo codigo "Curso de prueba" creditos num "Regular" "Profesor Prueba"
         (list (Horario "LUN" "07:30" "09:20"))
         '() '() 0 1 carrera))

(define comb-a
  (Combinacion (list (grupo-de "CE1103" 1 4 "Computadores")
                     (grupo-de "MA1403" 6 4 "Computadores"))
               8
               '("Computadores")))

(define comb-b
  (Combinacion (list (grupo-de "EM5001" 2 4 "Enseñanza de la Matematica"))
               4
               '("Enseñanza de la Matematica")))

;; formato de una línea
(check-equal? (combinacion->linea comb-a)
              "CE1103-G1|MA1403-G6;8;Computadores")

;; exportación completa: encabezado + una línea por combinación
(define ruta-prueba (build-path (find-system-path 'temp-dir)
                                "combinaciones-test.csv"))
(check-equal? (exportar-combinaciones (stream comb-a comb-b) ruta-prueba) 2)
(check-equal? (file->lines ruta-prueba)
              (list "cursos;creditos_totales;carreras"
                    "CE1103-G1|MA1403-G6;8;Computadores"
                    "EM5001-G2;4;Enseñanza de la Matematica"))

;; stream vacío: solo encabezado y devuelve 0
(check-equal? (exportar-combinaciones empty-stream ruta-prueba) 0)
(check-equal? (file->lines ruta-prueba)
              (list "cursos;creditos_totales;carreras"))

(displayln "salida-tests: todas las pruebas pasaron")
