#lang racket

;; lector-catalogo.rkt
;;
;; Lee el catálogo CSV que exporta el módulo de C (output/catalogo_salida.csv).
;; Formato (separador ';', una línea por grupo, primera línea = encabezado):
;;
;;   codigo;nombre;creditos;grupo;tipo;profesor;horarios;requisitos;
;;   correquisitos;choca;puede_matricular;carrera
;;
;; horarios: bloques separados por '|', ej. "MIE 07:30-09:20|VIE 07:30-09:20"
;; requisitos / correquisitos: códigos separados por ',' (vacío si no hay)

(require "tipos.rkt")

(provide leer-catalogo
         agrupar-por-curso
         obtener-grupos-por-codigo
         parsear-horarios)

;; Divide una cadena por un carácter separador. Recursivo, sin mutación.
(define (dividir s separador)
  (let loop ([i 0] [inicio 0] [acc '()])
    (cond
      [(>= i (string-length s))
       (reverse (cons (substring s inicio i) acc))]
      [(char=? (string-ref s i) separador)
       (loop (+ i 1) (+ i 1) (cons (substring s inicio i) acc))]
      [else (loop (+ i 1) inicio acc)])))

;; Cadena vacía -> lista vacía; si no, dividir por el separador.
(define (dividir-o-vacio s separador)
  (if (string=? (string-trim s) "")
      '()
      (dividir s separador)))

;; "MIE 07:30-09:20" -> Horario "MIE" "07:30" "09:20"
(define (parsear-bloque bloque)
  (let ([partes (dividir (string-trim bloque) #\space)])
    (if (= (length partes) 2)
        (let ([horas (dividir (cadr partes) #\-)])
          (if (= (length horas) 2)
              (Horario (car partes) (car horas) (cadr horas))
              (Horario (string-trim bloque) "" "")))
        (Horario (string-trim bloque) "" ""))))

(define (parsear-horarios s)
  (map parsear-bloque (dividir-o-vacio s #\|)))

(define (a-entero s)
  (let ([n (string->number (string-trim s))])
    (if (and n (exact-nonnegative-integer? (inexact->exact n)))
        (inexact->exact n)
        0)))

(define (campo campos i)
  (if (> (length campos) i)
      (string-trim (list-ref campos i))
      ""))

;; Convierte una línea del CSV en un Grupo.
(define NUMERO-CAMPOS 12)

(define (linea->grupo linea)
  (let ([campos (dividir linea #\;)])
    (unless (= (length campos) NUMERO-CAMPOS)
      (error 'leer-catalogo
             "se esperaban ~a campos y llegaron ~a (¿falta la columna carrera de C?): ~a"
             NUMERO-CAMPOS (length campos) linea))
    (Grupo (campo campos 0)
           (campo campos 1)
           (a-entero (campo campos 2))
           (a-entero (campo campos 3))
           (campo campos 4)
           (campo campos 5)
           (parsear-horarios (campo campos 6))
           (dividir-o-vacio (campo campos 7) #\,)
           (dividir-o-vacio (campo campos 8) #\,)
           (a-entero (campo campos 9))
           (a-entero (campo campos 10))
           (campo campos 11))))

;; Lee el archivo completo y devuelve la lista de Grupo (sin el encabezado).
(define (leer-catalogo ruta)
  (let* ([contenido (file->string ruta)]
         [lineas (filter (lambda (l) (not (string=? (string-trim l) "")))
                         (regexp-split #rx"\r\n|\n|\r" contenido))])
    (if (null? lineas)
        '()
        (map linea->grupo (cdr lineas)))))

;; Agrupa los grupos por código: lista de (codigo . lista-de-Grupo).
(define (agrupar-por-curso grupos)
  (map (lambda (cod)
         (cons cod (obtener-grupos-por-codigo grupos cod)))
       (remove-duplicates (map Grupo-codigo grupos))))

;; Filtra los grupos de un código dado.
(define (obtener-grupos-por-codigo grupos cod)
  (filter (lambda (g) (string=? (Grupo-codigo g) cod)) grupos))
