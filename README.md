# CEmestre - Etapa 1 (C)

Proyecto del curso Paradigmas de Programación (CE1106). Este módulo carga el
catálogo de cursos y el historial del estudiante, detecta choques de horario,
valida requisitos y correquisitos, y exporta el catálogo a un archivo de salida.

## Arquitectura del proyecto

(pendiente)

## Estructuras de datos

(pendiente)

## Persona 2: Detección de Choques de Horario (Ian Alejandro Bonilla Mena)

### 1. Algoritmo de detección de choques

Para determinar los conflictos de horarios entre los cursos del catálogo, se diseñó un algoritmo basado en ciclos iterativos anidados con optimizaciones de rendimiento y aislamiento de memoria:

- Optimización de comparaciones: En la función maestra detectarChoques, el ciclo iterativo secundario se inicializa partiendo de j = i + 1. Esto evita que el sistema compare un curso consigo mismo y elimina las comparaciones redundantes (evaluar el curso A contra el B produce el mismo resultado que evaluar el B contra el A).
- Salida temprana (Short-circuit): Se implementó la bandera booleana !hayChoque en la evaluación de los grupos. El algoritmo detiene la iteración en el instante exacto en que detecta el primer conflicto, ahorrando ciclos de procesamiento innecesarios.
- Aislamiento de estructuras: En lugar de alterar la estructura Curso diseñada por la Persona 1, los resultados de los choques se almacenan y manejan a través de un arreglo paralelo independiente (resultadosChoques).

### 2. Casos Límite

Se identificó y resolvió el caso límite donde un curso finaliza exactamente a la misma hora en que inicia otro (por ejemplo, un curso de Lunes de 8:00 a 10:00 y otro de Lunes de 10:00 a 12:00).

Solución técnica: Se empleó el operador lógico de desigualdad estricta (<) en la validación de superposición matemática: (h1.horaInicio < h2.horaFin) && (h2.horaInicio < h1.horaFin). Al comparar el final del primer curso (1000) con el inicio del segundo (1000), la condición 1000 < 1000 se evalúa como falsa, garantizando que el sistema no lo reporte erróneamente como un choque.

### 3. Decisiones de diseño sobre horarios

- Uso de tipos primitivos (int): Se decidió trabajar directamente con las horas representadas como números enteros en lugar de cadenas de texto (strings). Esta decisión técnica eliminó la necesidad de realizar costosas conversiones de datos en tiempo de ejecución (como el uso de atoi), facilitando una validación matemática directa.
- Indicador Booleano para el output: Aunque el proyecto planteaba la posibilidad de guardar la lista completa de nombres de cursos que chocan, se decidió simplificar el retorno a un indicador binario (1 o 0). Esta decisión satisface estrictamente los requerimientos mínimos del archivo de salida ("Si el curso choca con al menos otro curso/grupo"), garantizando un catálogo final más ligero y óptimo para ser procesado por la siguiente etapa en Racket.

## Persona 3: Validación de requisitos, correquisitos y salida (Bryan Sibaja)

### Validación de requisitos y correquisitos

Las reglas que aplicamos para decidir si un curso se puede matricular son:

1. Un requisito se cumple solo si el curso aparece en el historial de aprobados.
2. Un correquisito se cumple si ya está aprobado, o si el estudiante puede
   matricularlo en el mismo semestre (existe en el catálogo y cumple sus
   propios requisitos).
3. Un curso que ya está aprobado no se puede volver a matricular, así que
   `puede_matricular` queda en 0 aunque cumpla todo lo demás.

**Caso límite real:** en nuestro dataset QU1102 y QU1106 son correquisitos
mutuos (cada uno pide al otro). Si el correquisito se validara igual que un
requisito (exigiendo que ya esté aprobado), ningún estudiante podría matricular
ninguno de los dos nunca. Por eso el correquisito acepta también un curso que
sea matriculable en el mismo semestre, que es lo que pasa en la matrícula real.

### Formato de salida

El catálogo se exporta a `output/catalogo_salida.csv` en CSV separado por `;`,
con una línea por cada grupo de cada curso:

```
codigo;nombre;creditos;grupo;tipo;profesor;horarios;requisitos;correquisitos;choca;puede_matricular
```

- `horarios`: bloques separados por `|`, por ejemplo `MIE 07:30-09:20|VIE 07:30-09:20`.
- `requisitos` y `correquisitos`: códigos separados por coma, vacío si no hay.
- `choca`: 1 si el curso choca con al menos otro curso/grupo del catálogo
  (viene del arreglo paralelo `resultadosChoques` de la Persona 2, por lo que
  se repite en todas las líneas del mismo curso).
- `puede_matricular`: 1 si el estudiante cumple requisitos y correquisitos.

Por qué CSV con `;`:

- Es el mismo formato y separador del archivo de entrada, así todo el proyecto
  maneja una sola convención.
- La etapa 2 (Racket) puede leerlo línea por línea y partir cada campo con
  `string-split`, sin necesitar ninguna librería.
- Una línea por grupo evita anidar estructuras dentro de una celda: para armar
  combinaciones de horario, Racket necesita trabajar a nivel de grupo, no de
  curso.

### Historial del estudiante

`data/historial.csv` tiene un código de curso aprobado por línea, sin
encabezado. El archivo de ejemplo representa un estudiante de Computadores con
los primeros dos semestres aprobados (CE1101, CE1104, MA0101, MA1102, MA1403,
FI1101, FI1201, CS1502), lo que permite probar los tres resultados posibles:
cursos elegibles (CE1103, CE1105, FI1102), cursos bloqueados por requisitos
(CE2103, MA2104) y cursos ya aprobados que no se vuelven a matricular.
