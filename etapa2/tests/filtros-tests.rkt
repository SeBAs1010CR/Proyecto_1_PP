#lang racket

;; Pruebas del módulo de filtros.

(require rackunit
         racket/stream
         "../src/filtros.rkt"
         "../src/tipos.rkt")

;; filtra números
(check-equal? (stream->list (filtrar-stream even? (stream 1 2 3 4 5 6)))
              '(2 4 6))

;; filtra strings (mismo filtro, otro tipo de elemento)
(check-equal? (stream->list (filtrar-stream (lambda (s) (> (string-length s) 2))
                                            (stream "a" "abc" "ab" "abcd")))
              '("abc" "abcd"))

;; stream vacío
(check-true (stream-empty? (filtrar-stream even? empty-stream)))

;; ningún elemento cumple
(check-true (stream-empty? (filtrar-stream negative? (stream 1 2 3))))

;; es perezoso: filtrar un stream infinito no se cuelga
(define (naturales-desde n) (stream-cons n (naturales-desde (add1 n))))
(check-equal? (stream->list (stream-take (filtrar-stream even? (naturales-desde 0)) 4))
              '(0 2 4 6))

;; combinación mínima para probar el filtro de créditos
(define (combinacion-de creditos)
  (Combinacion '() creditos '()))

(define preferencias (Preferencias '() 12 18))

;; conserva solo las combinaciones dentro del rango 12-18
(check-equal? (stream->list
               (stream-map Combinacion-creditos-totales
                           (filtrar-por-creditos
                            (stream (combinacion-de 11) (combinacion-de 12)
                                    (combinacion-de 15) (combinacion-de 18)
                                    (combinacion-de 19))
                            preferencias)))
              '(12 15 18))

;; los límites del rango son inclusivos
(check-true ((en-rango-creditos? 12 18) (combinacion-de 12)))
(check-true ((en-rango-creditos? 12 18) (combinacion-de 18)))
(check-false ((en-rango-creditos? 12 18) (combinacion-de 11)))
(check-false ((en-rango-creditos? 12 18) (combinacion-de 19)))

(displayln "filtros-tests: todas las pruebas pasaron")
