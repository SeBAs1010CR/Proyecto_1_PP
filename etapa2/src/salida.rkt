#lang racket

;; salida.rkt
;;
;; Exporta las combinaciones filtradas al archivo que consumirá la
;; etapa 3 (Prolog). CSV separado por ';', una línea por combinación:
;;   cursos;creditos_totales;carreras
;; donde cursos = pares curso-grupo unidos con '|' (CE1103-G1|MA1403-G6)
;; y carreras = carreras de origen unidas con '|'.

(require racket/stream
         "tipos.rkt")

(provide (contract-out
          [combinacion->linea (-> Combinacion? string?)]
          [exportar-combinaciones (-> stream? path-string?
                                      exact-nonnegative-integer?)]))

(define ENCABEZADO "cursos;creditos_totales;carreras")

;; Par curso-grupo en texto: "CE1103-G1".
(define (grupo->texto grupo)
  (format "~a-G~a" (Grupo-codigo grupo) (Grupo-grupo-num grupo)))

;; Convierte una combinación en su línea del CSV.
(define (combinacion->linea combinacion)
  (string-join
   (list (string-join (map grupo->texto (Combinacion-grupos combinacion)) "|")
         (number->string (Combinacion-creditos-totales combinacion))
         (string-join (Combinacion-carreras combinacion) "|"))
   ";"))

;; Escribe cada combinación del stream como una línea, con recursión de
;; cola. Devuelve la cantidad de líneas escritas.
(define (escribir-combinaciones puerto combinaciones escritas)
  (if (stream-empty? combinaciones)
      escritas
      (begin
        (displayln (combinacion->linea (stream-first combinaciones)) puerto)
        (escribir-combinaciones puerto (stream-rest combinaciones)
                                (add1 escritas)))))

;; Exporta el stream de combinaciones a la ruta indicada.
;; Devuelve la cantidad de combinaciones exportadas.
(define (exportar-combinaciones combinaciones ruta)
  (call-with-output-file ruta
    (lambda (puerto)
      (displayln ENCABEZADO puerto)
      (escribir-combinaciones puerto combinaciones 0))
    #:exists 'replace))
