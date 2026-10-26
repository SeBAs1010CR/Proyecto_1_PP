#lang racket

;; ============================================================================
;; MÓDULO PRINCIPAL (PUNTO DE ENTRADA) - Etapa 2 (Integración Completa)
;;
;; Descripción: Este script orquesta la tubería de datos (pipeline) del 
;; sistema. Se encarga de capturar los argumentos de la consola, invocar la 
;; lectura y validación de datos (Rol 1), pasar el catálogo al motor de 
;; generación por streams (Rol 2), y aplicar el filtrado y exportación (Rol 3).
;; ============================================================================

(require "src/tipos.rkt"
         "src/lector-catalogo.rkt"
         "src/lector-preferencias.rkt"
         "src/generador.rkt"
         "src/filtros.rkt"
         "src/salida.rkt")

;; ============================================================================
;; CONFIGURACIÓN DE RUTAS Y ARGUMENTOS
;; ============================================================================

;; Captura los argumentos pasados por consola al ejecutar el script.
(define argumentos (current-command-line-arguments))

;; Define la ruta del catálogo. Usa el primer argumento si existe, 
;; de lo contrario usa la ruta por defecto del archivo generado en C.
(define ruta-catalogo
  (if (> (vector-length argumentos) 0)
      (vector-ref argumentos 0)
      "../output/catalogo_salida.csv"))

;; Define la ruta de las preferencias. Usa el segundo argumento si existe,
;; de lo contrario asume la ruta por defecto en la carpeta data.
(define ruta-preferencias
  (if (> (vector-length argumentos) 1)
      (vector-ref argumentos 1)
      "data/preferencias.csv"))

;; ============================================================================
;; FUNCIÓN PRINCIPAL DE ORQUESTACIÓN
;; ============================================================================

;; Ejecuta el flujo completo del programa coordinando los tres módulos.
(define (main)
  (printf "=== CEmestre - Etapa 2 (Ejecución de Pipeline Completo) ===\n\n")
  
  ;; FASE 1: LECTURA Y TIPADO (Rol 1)
  (let* ([grupos (leer-catalogo ruta-catalogo)]
         [por-curso (agrupar-por-curso grupos)]
         [pref (leer-preferencias ruta-preferencias)]
         [errores (validar-preferencias pref)])
    
    ;; Reporte del estado inicial de los datos
    (printf "Catálogo: ~a grupos, ~a cursos únicos\n" (length grupos) (length por-curso))
    (printf "Carreras: ~a\n" (remove-duplicates (map Grupo-carrera grupos)))
    (printf "Créditos deseados: ~a - ~a\n"
            (Preferencias-creditos-min pref)
            (Preferencias-creditos-max pref))
    (printf "Cursos de interés (~a):\n" (length (Preferencias-cursos-interes pref)))
    
    (for ([ci (Preferencias-cursos-interes pref)])
      (printf "  ~a GRUPO ~a\n" (car ci) (cdr ci)))
      
    (if (null? errores)
        (begin
          (printf "\nPreferencias válidas. Iniciando pipeline funcional...\n")
          
          ;; Se utiliza let* para declarar secuencialmente las variables locales del pipeline.
          (let* (
                 ;; FASE 2: GENERACIÓN CON EVALUACIÓN PEREZOSA (Rol 2)
                 ;; Le pasamos el catálogo y las preferencias (pref) para que el generador 
                 ;; extraiga los "cursos de interés" y evite la explosión combinatoria.
                 [stream-combinaciones (generar-horarios-validos grupos pref)]
                 
                 ;; FASE 3: FILTRADO POLIMÓRFICO (Rol 3)
                 ;; Aplica el filtro sobre el stream para cumplir con el rango de créditos.
                 [stream-filtrado (filtrar-por-creditos stream-combinaciones pref)]
                 
                 ;; FASE 4: EVALUACIÓN Y EXPORTACIÓN (Rol 3)
                 ;; Consume el stream recursivamente y escribe cada combinación válida en el CSV.
                 [ruta-salida "../output/combinaciones.csv"]
                 [total-exportadas (exportar-combinaciones stream-filtrado ruta-salida)])
                 
            (printf "\n¡Éxito! Se generaron y exportaron ~a combinaciones válidas en: ~a\n" 
                    total-exportadas ruta-salida)))
                  
        ;; Si las preferencias tienen errores, se aborta la generación y se muestran.
        (for ([e errores])
          (printf "\n[Advertencia] ~a\n" e)))))

;; ============================================================================
;; EJECUCIÓN CON MANEJO DE EXCEPCIONES
;; ============================================================================

;; Atrapa cualquier error fatal durante la ejecución y lo imprime de forma limpia.
(with-handlers ([exn:fail? (lambda (e)
                             (printf "Error fatal durante la ejecución: ~a\n" (exn-message e)))])
  (main))