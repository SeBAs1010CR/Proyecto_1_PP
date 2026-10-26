#lang racket

;; ============================================================================
;; MÓDULO DE GENERACIÓN DE COMBINACIONES (Rol 2)
;; Autor: Ian Alejandro Bonilla Mena
;; ============================================================================

(require racket/stream)
(require racket/list)
(require "tipos.rkt")

(provide generar-horarios-validos)

;; ============================================================================
;; SECCIÓN 1: LÓGICA DE DETECCIÓN DE CONFLICTOS
;; ============================================================================

(define (chocan-horarios? horario1 horario2)
  (and (equal? (Horario-dia horario1) (Horario-dia horario2))
       (not (or (string<=? (Horario-hora-fin horario1) (Horario-hora-inicio horario2))
                (string>=? (Horario-hora-inicio horario1) (Horario-hora-fin horario2))))))

(define (choca-con-horario? nuevo-grupo combinacion-actual)
  (define (chocan-grupos? grupo-a grupo-b)
    (ormap (lambda (horario-a)
             (ormap (lambda (horario-b) 
                      (chocan-horarios? horario-a horario-b))
                    (Grupo-horarios grupo-b)))
           (Grupo-horarios grupo-a)))
  (ormap (lambda (grupo-existente) 
           (chocan-grupos? nuevo-grupo grupo-existente)) 
         combinacion-actual))

;; ============================================================================
;; SECCIÓN 2: CONSTRUCCIÓN DE ESTRUCTURAS DE SALIDA
;; ============================================================================

(define (crear-combinacion grupos-seleccionados)
  (Combinacion 
   grupos-seleccionados
   (apply + (map Grupo-creditos grupos-seleccionados))
   (remove-duplicates (map Grupo-carrera grupos-seleccionados))))

;; ============================================================================
;; SECCIÓN 3: MOTOR DE GENERACIÓN MEDIANTE STREAMS
;; ============================================================================

;; Ahora recibimos tanto el catálogo completo como las preferencias del estudiante.
(define (generar-horarios-validos lista-plana-grupos preferencias)
  (define cursos-interes (Preferencias-cursos-interes preferencias))
  
  ;; 1. Filtramos el catálogo masivo para quedarnos ÚNICAMENTE con 
  ;; los cursos y grupos específicos que el estudiante solicitó.
  (define grupos-filtrados
    (filter (lambda (g)
              (ormap (lambda (interes)
                       (and (equal? (Grupo-codigo g) (car interes))
                            (equal? (Grupo-grupo-num g) (cdr interes))))
                     cursos-interes))
            lista-plana-grupos))
  
  ;; 2. Agrupamos los grupos filtrados por código de curso.
  (define cursos-agrupados (group-by Grupo-codigo grupos-filtrados))
  
  ;; 3. Iniciamos la generación con el subconjunto.
  (generar-aux cursos-agrupados '()))

(define (generar-aux cursos-restantes combinacion-actual)
  (if (empty? cursos-restantes)
      ;; CASO BASE: Empaquetamos el subconjunto generado
      (stream (crear-combinacion combinacion-actual))
      
      ;; CASO RECURSIVO: Evaluamos los grupos del siguiente curso de interés
      (let ([grupos-del-curso (first cursos-restantes)])
        (stream-append
         ;; RAMA 1: Decidimos NO matricular este curso (permite generar subconjuntos de menor tamaño)
         (generar-aux (rest cursos-restantes) combinacion-actual)
         
         ;; RAMA 2: Decidimos SÍ matricular este curso
         (foldr (lambda (grupo-candidato stream-acumulado)
                  (if (choca-con-horario? grupo-candidato combinacion-actual)
                      stream-acumulado 
                      (stream-append 
                       (generar-aux (rest cursos-restantes) (cons grupo-candidato combinacion-actual))
                       stream-acumulado)))
                empty-stream
                grupos-del-curso)))))