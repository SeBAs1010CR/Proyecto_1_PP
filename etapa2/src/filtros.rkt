#lang racket

;; filtros.rkt
;;
;; Filtrado perezoso de streams. filtrar-stream es una función con
;; polimorfismo paramétrico: funciona sobre un stream de cualquier tipo
;; de elemento, porque solo depende del predicado que recibe.

(require racket/stream
         "tipos.rkt")

(provide (contract-out
          [filtrar-stream (-> (-> any/c boolean?) stream? stream?)]
          [en-rango-creditos? (-> exact-nonnegative-integer?
                                  exact-nonnegative-integer?
                                  (-> Combinacion? boolean?))]
          [filtrar-por-creditos (-> stream? Preferencias? stream?)]))

;; Conserva los elementos del stream que cumplen el predicado.
;; Es perezoso: no evalúa más allá de lo que el consumidor pida.
(define (filtrar-stream cumple? elementos)
  (cond
    [(stream-empty? elementos) empty-stream]
    [(cumple? (stream-first elementos))
     (stream-cons (stream-first elementos)
                  (filtrar-stream cumple? (stream-rest elementos)))]
    [else (filtrar-stream cumple? (stream-rest elementos))]))

;; Devuelve un predicado que acepta las combinaciones cuyos créditos
;; totales están dentro del rango [minimo, maximo].
(define (en-rango-creditos? minimo maximo)
  (lambda (combinacion)
    (and (>= (Combinacion-creditos-totales combinacion) minimo)
         (<= (Combinacion-creditos-totales combinacion) maximo))))

;; Aplica el filtro de créditos de las preferencias del estudiante
;; sobre el stream de combinaciones que genera el Rol 2.
(define (filtrar-por-creditos combinaciones preferencias)
  (filtrar-stream (en-rango-creditos? (Preferencias-creditos-min preferencias)
                                      (Preferencias-creditos-max preferencias))
                  combinaciones))
