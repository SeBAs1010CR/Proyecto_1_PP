#lang racket

;; ============================================================================
;; MÓDULO DE GENERACIÓN DE COMBINACIONES (Rol 2)
;; Autor: Ian Alejandro Bonilla Mena
;;
;; Descripción: Este módulo es el motor central (núcleo algorítmico) encargado 
;; de generar todas las combinaciones de horarios posibles sin choques. 
;; Utiliza evaluación perezosa (streams) para evitar la explosión combinatoria 
;; y el desbordamiento de memoria. El diseño es puramente funcional.
;; ============================================================================

(require racket/stream)
(require racket/list)
(require "tipos.rkt")

(provide generar-horarios-validos)

;; ============================================================================
;; SECCIÓN 1: LÓGICA DE DETECCIÓN DE CONFLICTOS
;; ============================================================================

;; Verifica si dos bloques de horario individuales se superponen en el tiempo.
;;
;; @param horario1 : Horario - Primer bloque de horario a evaluar.
;; @param horario2 : Horario - Segundo bloque de horario a evaluar.
;; @return         : boolean? - Retorna #t si existe un choque, #f en caso contrario.
(define (chocan-horarios? horario1 horario2)
  (and (equal? (Horario-dia horario1) (Horario-dia horario2))
       ;; Dos horarios NO chocan si uno termina antes o exactamente cuando el otro empieza,
       ;; o si uno empieza después o exactamente cuando el otro termina.
       ;; Al usar 'not' sobre esta condición, detectamos la superposición.
       ;; Nota: Se utiliza string<=? y string>=? porque las horas vienen como texto (ej. "07:30").
       (not (or (string<=? (Horario-hora-fin horario1) (Horario-hora-inicio horario2))
                (string>=? (Horario-hora-inicio horario1) (Horario-hora-fin horario2))))))

;; Verifica si un grupo candidato tiene algún conflicto de horario con los grupos
;; que ya han sido seleccionados en la combinación actual.
;;
;; @param nuevo-grupo        : Grupo - El grupo que se intenta añadir a la matrícula.
;; @param combinacion-actual : (listof Grupo) - Lista de grupos ya validados y seleccionados.
;; @return                   : boolean? - Retorna #t si el nuevo grupo choca con alguno existente.
(define (choca-con-horario? nuevo-grupo combinacion-actual)
  
  ;; Función auxiliar interna para comparar todos los horarios de dos grupos.
  (define (chocan-grupos? grupo-a grupo-b)
    ;; ormap implementa cortocircuito (short-circuit): se detiene al encontrar el primer #t
    (ormap (lambda (horario-a)
             (ormap (lambda (horario-b) 
                      (chocan-horarios? horario-a horario-b))
                    (Grupo-horarios grupo-b)))
           (Grupo-horarios grupo-a)))
  
  ;; Evalúa el nuevo grupo contra cada grupo ya consolidado en la combinación.
  (ormap (lambda (grupo-existente) 
           (chocan-grupos? nuevo-grupo grupo-existente)) 
         combinacion-actual))

;; ============================================================================
;; SECCIÓN 2: CONSTRUCCIÓN DE ESTRUCTURAS DE SALIDA
;; ============================================================================

;; Toma una lista de grupos válidos (sin choques) y la empaqueta en la 
;; estructura 'Combinacion' requerida por la siguiente etapa del pipeline.
;;
;; @param grupos-seleccionados : (listof Grupo) - Lista final de grupos para un horario.
;; @return                     : Combinacion - Estructura tipada con los datos totales.
(define (crear-combinacion grupos-seleccionados)
  (Combinacion 
   grupos-seleccionados
   ;; Calcula el total de créditos sumando los créditos individuales de cada grupo
   (apply + (map Grupo-creditos grupos-seleccionados))
   ;; Extrae las carreras que aportan estos cursos y elimina los valores duplicados
   (remove-duplicates (map Grupo-carrera grupos-seleccionados))))

;; ============================================================================
;; SECCIÓN 3: MOTOR DE GENERACIÓN MEDIANTE STREAMS
;; ============================================================================

;; Punto de entrada principal del módulo. Transforma la lista plana de grupos 
;; proveniente del archivo CSV en un stream de combinaciones válidas.
;;
;; @param lista-plana-grupos : (listof Grupo) - Catálogo completo de grupos disponibles.
;; @return                   : stream? - Flujo perezoso de estructuras Combinacion.
(define (generar-horarios-validos lista-plana-grupos)
  ;; Agrupamos la lista plana por el código del curso.
  ;; Resultado: una lista de listas, donde cada sublista tiene los grupos de un mismo curso.
  (define cursos-agrupados (group-by Grupo-codigo lista-plana-grupos))
  ;; Llamada inicial a la función recursiva con el acumulador vacío.
  (generar-aux cursos-agrupados '()))

;; Función recursiva pura que construye el stream de combinaciones utilizando 
;; evaluación perezosa para evitar desbordamientos de memoria (poda algorítmica).
;;
;; @param cursos-restantes   : (listof (listof Grupo)) - Cursos pendientes de evaluar.
;; @param combinacion-actual : (listof Grupo) - Grupos acumulados hasta el nivel actual.
;; @return                   : stream? - Flujo acumulado de combinaciones válidas.
(define (generar-aux cursos-restantes combinacion-actual)
  (if (empty? cursos-restantes)
      ;; CASO BASE: No quedan cursos por evaluar. La combinación actual está completa y es válida.
      ;; Se empaqueta en un stream de un solo elemento.
      (stream (crear-combinacion combinacion-actual))
      
      ;; CASO RECURSIVO: Extraemos todos los grupos del siguiente curso a evaluar.
      (let ([grupos-del-curso (first cursos-restantes)])
        
        ;; Iteramos sobre cada grupo disponible para este curso usando foldr
        (foldr (lambda (grupo-candidato stream-acumulado)
                 (if (choca-con-horario? grupo-candidato combinacion-actual)
                     ;; PODA TEMPRANA: Si el grupo choca, descartamos esta rama recursiva 
                     ;; y devolvemos el stream acumulado tal como está.
                     stream-acumulado 
                     
                     ;; CONTINUACIÓN: Si el grupo es válido, lo agregamos a la combinación,
                     ;; avanzamos al siguiente curso (rest) y concatenamos perezosamente.
                     (stream-append 
                      (generar-aux (rest cursos-restantes) (cons grupo-candidato combinacion-actual))
                      stream-acumulado)))
               empty-stream
               grupos-del-curso))))