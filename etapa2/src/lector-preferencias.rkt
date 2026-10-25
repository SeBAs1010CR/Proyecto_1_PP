#lang racket

;; lector-preferencias.rkt
;;
;; Lee el archivo de preferencias del estudiante. Formato:
;;
;;   cursos_interes
;;   CE1103,GRUPO 3
;;   CE1105,GRUPO 3
;;   creditos_min;creditos_max
;;   12;18
;;
;; La lista de interés debe tener más de un grupo (lo valida main.rkt con
;; la función validar-preferencias).

(require "tipos.rkt")

(provide leer-preferencias
         validar-preferencias)

(define (dividir s separador)
  (let loop ([i 0] [inicio 0] [acc '()])
    (cond
      [(>= i (string-length s))
       (reverse (cons (substring s inicio i) acc))]
      [(char=? (string-ref s i) separador)
       (loop (+ i 1) (+ i 1) (cons (substring s inicio i) acc))]
      [else (loop (+ i 1) inicio acc)])))

(define (a-entero s)
  (let ([n (string->number (string-trim s))])
    (if (and n (integer? n) (>= n 0))
        (inexact->exact n)
        #f)))

;; "CE1103,GRUPO 3" -> (cons "CE1103" 3), o #f si la línea no es válida
(define (parsear-interes linea)
  (let ([partes (dividir linea #\,)])
    (and (= (length partes) 2)
         (let ([codigo (string-trim (car partes))]
               [grupo (string-trim (cadr partes))])
           (and (string-prefix? grupo "GRUPO")
                (let ([num (a-entero (substring grupo 5))])
                  (and num (cons codigo num))))))))

;; Lee el archivo y devuelve una Preferencias.
;; Lanza error si el archivo no tiene un rango de créditos válido.
(define (leer-preferencias ruta)
  (let loop ([ls (regexp-split #rx"\r\n|\n|\r" (file->string ruta))]
             [intereses '()]
             [cmin #f]
             [cmax #f])
    (cond
      [(null? ls)
       (unless (and cmin cmax)
         (error 'leer-preferencias "falta el rango creditos_min;creditos_max"))
       (Preferencias (reverse intereses) cmin cmax)]
      [else
       (let ([linea (string-trim (car ls))])
         (cond
           [(or (string=? linea "")
                (string-prefix? linea "cursos_interes")
                (string-prefix? linea "creditos_min"))
            (loop (cdr ls) intereses cmin cmax)]
           [(string-contains? linea ";")
            (let ([partes (dividir linea #\;)])
              (if (= (length partes) 2)
                  (loop (cdr ls) intereses
                        (a-entero (car partes))
                        (a-entero (cadr partes)))
                  (loop (cdr ls) intereses cmin cmax)))]
           [(parsear-interes linea)
            => (lambda (par)
                 (loop (cdr ls) (cons par intereses) cmin cmax))]
           [else (loop (cdr ls) intereses cmin cmax)]))])))

;; Devuelve una lista de mensajes de error (vacía si todo es válido).
(define (validar-preferencias p)
  (append
   (if (> (Preferencias-creditos-min p) (Preferencias-creditos-max p))
       (list "creditos_min es mayor que creditos_max")
       '())
   (if (< (length (Preferencias-cursos-interes p)) 2)
       (list "la lista de interés debe tener al menos dos grupos")
       '())))
