#lang racket

;; main.rkt (Etapa 2, Rol 1: lectura y tipado)
;;
;; Uso (desde la carpeta etapa2/):
;;   racket main.rkt [ruta-catalogo] [ruta-preferencias]
;;
;; Por defecto lee ../output/catalogo_salida.csv (generado por C) y
;; data/preferencias.csv.

(require "src/tipos.rkt"
         "src/lector-catalogo.rkt"
         "src/lector-preferencias.rkt")

(define argumentos (current-command-line-arguments))

(define ruta-catalogo
  (if (> (vector-length argumentos) 0)
      (vector-ref argumentos 0)
      "../output/catalogo_salida.csv"))

(define ruta-preferencias
  (if (> (vector-length argumentos) 1)
      (vector-ref argumentos 1)
      "data/preferencias.csv"))

(define (main)
  (printf "=== CEmestre - Etapa 2 (Rol 1: lectura y tipado) ===\n\n")
  (let* ([grupos (leer-catalogo ruta-catalogo)]
         [por-curso (agrupar-por-curso grupos)]
         [pref (leer-preferencias ruta-preferencias)]
         [errores (validar-preferencias pref)])
    (printf "Catálogo: ~a grupos, ~a cursos únicos\n"
            (length grupos) (length por-curso))
    (printf "Carreras: ~a\n"
            (remove-duplicates (map Grupo-carrera grupos)))
    (printf "Créditos deseados: ~a - ~a\n"
            (Preferencias-creditos-min pref)
            (Preferencias-creditos-max pref))
    (printf "Cursos de interés (~a):\n"
            (length (Preferencias-cursos-interes pref)))
    (for ([ci (Preferencias-cursos-interes pref)])
      (printf "  ~a GRUPO ~a\n" (car ci) (cdr ci)))
    (if (null? errores)
        (printf "\nPreferencias válidas.\n")
        (for ([e errores])
          (printf "\n[Advertencia] ~a\n" e)))))

(with-handlers ([exn:fail? (lambda (e)
                             (printf "Error: ~a\n" (exn-message e)))])
  (main))
