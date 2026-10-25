#lang racket

;; Pruebas del módulo de filtros.

(require rackunit
         racket/stream
         "../src/filtros.rkt")

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

(displayln "filtros-tests: todas las pruebas pasaron")
