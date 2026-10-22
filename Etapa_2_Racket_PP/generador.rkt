#lang racket
(require racket/stream)

(provide generar-horarios-validos)

;; ============================================================================
;; SECCIÓN 1: DATOS DE PRUEBA (MOCK DATA)
;; ============================================================================
;; Nota: Estas estructuras son temporales. Deben ser reemplazadas por los tipos
;; explícitos que defina el Rol 1 al leer el archivo CSV real.

;; Representa un bloque de horario individual
(struct horario (dia inicio fin) #:transparent)

;; Representa un grupo específico de un curso, con su lista de horarios
(struct grupo (numero horarios) #:transparent)

;; Representa un curso con su lista de grupos disponibles
(struct curso (codigo grupos) #:transparent)

;; --- Datos quemados para probar el motor de streams ---
;; Creamos tres cursos. El grupo 1 de IC1802 choca con el grupo 1 de IC1803 los lunes.
(define curso1 (curso "IC1802" (list (grupo 1 (list (horario "L" 8 10))) 
                                     (grupo 2 (list (horario "K" 13 15))))))
(define curso2 (curso "IC1803" (list (grupo 1 (list (horario "L" 9 11)))))) 
(define curso3 (curso "MA3301" (list (grupo 1 (list (horario "J" 7 9))))))

;; Lista que simula el catálogo filtrado que entregará el Rol 1
(define catalogo-prueba (list curso1 curso2 curso3))

;; ============================================================================
;; SECCIÓN 2: LÓGICA DE DETECCIÓN DE CHOQUES
;; ============================================================================

;; Verifica si dos bloques de horario individuales se superponen.
;; Retorna #t si chocan, #f si no.
(define (chocan-horarios? h1 h2)
  (and (equal? (horario-dia h1) (horario-dia h2))
       ;; La lógica es: NO (termina antes que el otro empiece O empieza después que el otro termine)
       (not (or (<= (horario-fin h1) (horario-inicio h2))
                (>= (horario-inicio h1) (horario-fin h2))))))

;; Verifica si un grupo nuevo choca con algún grupo ya existente en la combinación actual.
;; nuevo-grupo: estructura 'grupo' que intentamos agregar.
;; combinacion-actual: lista de estructuras 'grupo' ya seleccionadas.
(define (choca-con-horario? nuevo-grupo combinacion-actual)
  (define (chocan-grupos? g1 g2)
    ;; ormap evalúa si AL MENOS UN horario de g1 choca con AL MENOS UN horario de g2
    (ormap (lambda (h1)
             (ormap (lambda (h2) (chocan-horarios? h1 h2))
                    (grupo-horarios g2)))
           (grupo-horarios g1)))
  
  ;; Compara el nuevo grupo contra todos los grupos ya guardados en la combinación
  (ormap (lambda (grupo-existente) 
           (chocan-grupos? nuevo-grupo grupo-existente)) 
         combinacion-actual))

;; ============================================================================
;; SECCIÓN 3: MOTOR DE GENERACIÓN CON STREAMS
;; ============================================================================

;; Función principal que recibe una lista de cursos y retorna un STREAM de combinaciones válidas.
(define (generar-horarios-validos cursos)
  (generar-aux cursos '()))

;; Función recursiva que construye el stream de forma perezosa.
;; cursos-restantes: lista de cursos pendientes por evaluar.
;; combinacion-actual: lista de grupos acumulados (sin choques).
(define (generar-aux cursos-restantes combinacion-actual)
  (if (empty? cursos-restantes)
      ;; CASO BASE: Si ya no hay cursos, la combinación es válida. Se retorna como un stream de 1 elemento.
      (stream combinacion-actual)
      
      ;; CASO RECURSIVO: Evaluamos los grupos del siguiente curso.
      (let* ([curso-actual (first cursos-restantes)]
             [grupos (curso-grupos curso-actual)])
        
        ;; foldr itera sobre los grupos del curso actual y acumula los resultados en un stream
        (foldr (lambda (grp acc-stream)
                 (if (choca-con-horario? grp combinacion-actual)
                     ;; Si el grupo choca, podamos esta rama devolviendo el stream acumulado sin cambios
                     acc-stream 
                     ;; Si NO choca, agregamos el grupo a la combinación, llamamos a la recursión
                     ;; y concatenamos el resultado al stream acumulado (stream-append)
                     (stream-append 
                      (generar-aux (rest cursos-restantes) (cons grp combinacion-actual))
                      acc-stream)))
               empty-stream
               grupos))))

;; ============================================================================
;; ZONA DE PRUEBAS (Descomentar para probar localmente)
;; ============================================================================
(displayln "Generando combinaciones válidas:")
;; ;; Convertimos el stream resultante a una lista solo para poder imprimirlo en consola
(define resultados (stream->list (generar-horarios-validos catalogo-prueba)))
(for-each (lambda (combo) (displayln combo)) resultados)