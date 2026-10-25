#lang racket

;; filtros.rkt
;;
;; Filtrado perezoso de streams. filtrar-stream es una función con
;; polimorfismo paramétrico: funciona sobre un stream de cualquier tipo
;; de elemento, porque solo depende del predicado que recibe.

(require racket/stream)

(provide (contract-out
          [filtrar-stream (-> (-> any/c boolean?) stream? stream?)]))

;; Conserva los elementos del stream que cumplen el predicado.
;; Es perezoso: no evalúa más allá de lo que el consumidor pida.
(define (filtrar-stream cumple? elementos)
  (cond
    [(stream-empty? elementos) empty-stream]
    [(cumple? (stream-first elementos))
     (stream-cons (stream-first elementos)
                  (filtrar-stream cumple? (stream-rest elementos)))]
    [else (filtrar-stream cumple? (stream-rest elementos))]))
