# CEmestre - Etapa 2 (Racket)

Módulo funcional del proyecto CEmestre. Lee el catálogo validado por la Etapa 1
(C) y el archivo de preferencias del estudiante, y deja los datos tipados
listos para generar y filtrar combinaciones de horario.

Esta documentación cubre la lectura y tipado (Rol 1) y el filtrado y
exportación (Rol 3). La generación de combinaciones se documenta en su
sección aparte cuando se complete.

## Ejecución

Desde la carpeta `etapa2/`:

```
racket main.rkt [ruta-catalogo] [ruta-preferencias]
```

- Por defecto lee `../output/catalogo_salida.csv` (generado por C) y
  `data/preferencias.csv`.
- El catálogo debe generarse primero con el programa de C desde la raíz del
  proyecto.

## Arquitectura

```
etapa2/
├── main.rkt                    Punto de entrada: carga y muestra los datos
├── src/
│   ├── tipos.rkt               Estructuras de datos con contratos
│   ├── lector-catalogo.rkt     Lee output/catalogo_salida.csv
│   ├── lector-preferencias.rkt Lee el archivo de preferencias y lo valida
│   ├── filtros.rkt             Filtro genérico de streams y filtro por créditos
│   └── salida.rkt              Exporta las combinaciones para la etapa 3
├── tests/
│   ├── filtros-tests.rkt       Pruebas del filtrado (rackunit)
│   └── salida-tests.rkt        Pruebas de la exportación (rackunit)
└── data/
    └── preferencias.csv        Ejemplo de preferencias del estudiante
```

Flujo: `catalogo_salida.csv` → `leer-catalogo` → lista de `Grupo`; y
`preferencias.csv` → `leer-preferencias` → `Preferencias`. Ambos pasan a los
módulos siguientes sin modificarse. Luego el generador (Rol 2) produce un
stream de `Combinacion`, `filtrar-por-creditos` descarta las que no calzan con
las preferencias y `exportar-combinaciones` escribe el archivo final.

## Estructuras de datos desarrolladas

Todas las estructuras están en `src/tipos.rkt`, se definen con `struct/contract`
y son inmutables (sin campos mutables ni `set!`). Si un dato no cumple el
contrato, falla al construirse indicando el campo.

### Horario

Un bloque horario de un grupo.

| Campo        | Tipo     | Ejemplo  |
|--------------|----------|----------|
| `dia`        | string   | `"MIE"`  |
| `hora-inicio`| string   | `"07:30"`|
| `hora-fin`   | string   | `"09:20"`|

### Grupo

Un grupo de un curso, tal como lo exporta el módulo de C.

| Campo               | Tipo                     |
|---------------------|--------------------------|
| `codigo`            | string                   |
| `nombre`            | string                   |
| `creditos`          | entero no negativo       |
| `grupo-num`         | entero no negativo       |
| `tipo`              | string                   |
| `profesor`          | string                   |
| `horarios`          | lista de `Horario`       |
| `requisitos`        | lista de strings         |
| `correquisitos`     | lista de strings         |
| `choca`             | `0` o `1`                |
| `puede-matricular`  | `0` o `1`                |
| `carrera`           | string                   |

### Preferencias

Lo que el estudiante quiere matricular.

| Campo           | Tipo                                      |
|-----------------|-------------------------------------------|
| `cursos-interes`| lista de pares `(codigo . numero-grupo)` |
| `creditos-min`  | entero no negativo                        |
| `creditos-max`  | entero no negativo                        |

### Combinacion

Definida para el Rol de generación. Una combinación es un grupo por curso, con
sus créditos totales y las carreras de origen.

| Campo              | Tipo                  |
|--------------------|-----------------------|
| `grupos`           | lista de `Grupo`      |
| `creditos-totales` | entero no negativo    |
| `carreras`         | lista de strings      |

## Formato de entrada

### Catálogo (`output/catalogo_salida.csv`)

Separador `;`, una línea por grupo, primera línea de encabezado:

```
codigo;nombre;creditos;grupo;tipo;profesor;horarios;requisitos;correquisitos;choca;puede_matricular;carrera
```

- `horarios`: bloques separados por `|`, por ejemplo `MIE 07:30-09:20|VIE 07:30-09:20`.
- `requisitos` y `correquisitos`: códigos separados por `,` (vacío si no hay).
- `carrera`: la carrera que aporta el curso (ver decisiones).

El lector rechaza líneas con un número de campos distinto de 12 y avisa que
probablemente falta la columna `carrera`.

### Preferencias (`data/preferencias.csv`)

```
cursos_interes
CE1101,GRUPO 3
CE1103,GRUPO 3
MA1403,GRUPO 6
creditos_min;creditos_max
12;18
```

La lista de intereses debe tener al menos dos grupos; `validar-preferencias`
revisa también que `creditos_min` no sea mayor que `creditos_max`.

## Generación de Combinaciones (Rol 2 - Ian Alejandro Bonilla Mena)

### Arquitectura Funcional del Proyecto

Para esta etapa en Racket, el sistema se diseñó bajo un modelo de tubería 
de datos (pipeline) puramente funcional, asegurando la inmutabilidad y la 
ausencia de efectos secundarios (sin uso de `set!` ni variables globales 
reasignadas). La arquitectura divide la solución en tres módulos aislados 
que se conectan a través de `main.rkt`:

1. **Entrada y Tipado:** Un módulo que lee el archivo serializado de la 
   Etapa 1 y lo transforma en estructuras de Racket fuertemente tipadas.
2. **Generación (Este componente):** Un motor central, implementado en 
   `generador.rkt`, que recibe el catálogo limpio y produce un flujo de 
   datos (stream) continuo con las combinaciones de horarios válidos.
3. **Filtrado y Salida:** Un módulo final que consume el stream, descarta 
   las combinaciones fuera del rango de créditos y serializa el resultado 
   para Prolog. 

El estado del programa nunca se modifica; en su lugar, se transfiere 
mediante parámetros en funciones recursivas a lo largo del flujo de ejecución.

### Algoritmo de Generación mediante Streams

El reto principal consistía en evitar la explosión combinatoria. Generar el 
producto cartesiano de todos los grupos de todos los cursos almacenando 
listas completas en memoria saturaría el sistema rápidamente. Para 
solucionarlo, se implementó un motor de evaluación perezosa (*lazy 
evaluation*) utilizando la librería `racket/stream`.

- **Poda algorítmica temprana (Pruning):** El algoritmo recursivo evalúa un 
  curso a la vez. Mediante el uso de `foldr`, el sistema intenta agregar 
  cada grupo a la `combinacion-actual`. Si la función `choca-con-horario?` 
  detecta un conflicto, esa rama de recursión devuelve el stream acumulado 
  sin procesar los cursos restantes, podando inmediatamente millones de 
  combinaciones inválidas antes de que se calculen.
- **Flujo bajo demanda:** Utilizando `stream-append` y la naturaleza 
  recursiva del algoritmo, el motor no devuelve una lista, sino una promesa 
  de cálculo. Las combinaciones válidas solo se generan en memoria a medida 
  que el módulo de filtrado (Persona 3) o la exportación las va solicitando.

### Casos Límite

**Caso límite real:** Se detectó un problema de rendimiento exponencial 
cuando un curso poseía múltiples grupos con horarios complejos (ej. clases 
teóricas de 2 días y laboratorios de 1 día) y el algoritmo intentaba 
validarlos contra un horario que ya tenía 4 o 5 cursos matriculados. Realizar 
iteraciones tradicionales sobre todas esas listas anidadas generaba cuellos 
de botella temporales.

**Solución técnica:** Se reestructuró la validación matemática de choques 
heredada de C, transformándola en una composición de funciones de orden 
superior (`ormap`). Al anidar llamadas a `ormap` (para iterar sobre los 
horarios del grupo nuevo, y a su vez sobre los horarios de los grupos ya 
matriculados), el lenguaje aplica una evaluación de cortocircuito 
(short-circuit) de forma nativa. En el instante exacto en que la expresión 
lógica de superposición de horas (que valida la hora de inicio y fin) 
detecta una colisión en un solo día, el `ormap` interno retorna `#t` y 
detiene inmediatamente la evaluación de los días restantes de la semana y de 
los otros cursos, optimizando drásticamente el tiempo de procesamiento de 
cada rama del stream.

### Decisiones de Diseño Funcional

- **Ausencia de estado global en el horario:** A diferencia de una matriz 
  semanal o un arreglo global donde se marcan las horas ocupadas, se 
  decidió representar el "horario actual" simplemente como una lista 
  enlazada de las estructuras `grupo` seleccionadas en la rama de recursión 
  vigente. Esto garantiza la pureza de la función de generación.
- **Aislamiento de la lógica de evaluación:** Se separó estrictamente la 
  función `chocan-horarios?` (que evalúa dos bloques individuales) de la 
  función `choca-con-horario?` (que evalúa listas de grupos). Este diseño 
  modular permite que la lógica base sea testeada de forma aislada e 
  independiente de los datos temporales o de las estructuras definitivas 
  inyectadas por el módulo de lectura.

## Filtrado y polimorfismo

El filtrado vive en `src/filtros.rkt` y son tres funciones encadenadas:

- `filtrar-stream` es la función con **polimorfismo paramétrico**: recibe un
  predicado y un stream de cualquier tipo de elemento y devuelve el stream con
  los elementos que cumplen. No conoce nada de cursos ni combinaciones, por eso
  las pruebas la ejercitan igual con números, con strings y con `Combinacion`.
  Está implementada con recursión propia sobre `stream-cons`, así que es
  perezosa: no evalúa más combinaciones de las que el consumidor pida (las
  pruebas la aplican sobre un stream infinito sin colgarse).
- `en-rango-creditos?` recibe el mínimo y el máximo de las preferencias y
  **devuelve un predicado** sobre combinaciones. Los límites son inclusivos:
  una combinación de exactamente `creditos_min` o `creditos_max` créditos sí
  se acepta.
- `filtrar-por-creditos` une las dos anteriores: toma el stream del generador
  y las `Preferencias`, y devuelve el stream ya filtrado. Es la prueba de que
  el polimorfismo "es útil para el programa": el filtro de créditos no tiene
  lógica de streams propia, solo le pasa un predicado a la función genérica.

## Formato de salida

`src/salida.rkt` exporta las combinaciones filtradas en CSV separado por `;`,
una línea por combinación, con encabezado:

```
cursos;creditos_totales;carreras
CE1103-G1|MA1403-G6;8;Computadores
```

- `cursos`: pares curso-grupo (`codigo-Gnumero`) unidos con `|`.
- `creditos_totales`: suma de créditos de la combinación.
- `carreras`: carreras de origen de los cursos, unidas con `|`.

**Justificación.** La decisión real detrás del formato es que la etapa 3
(Prolog) consume cada combinación como una unidad: una línea por combinación
permite convertir cada línea en un hecho Prolog (por ejemplo
`combinacion(["CE1103-G1","MA1403-G6"], 8, ...)`) sin reconstruir nada a
partir de varias líneas. Además se mantienen las convenciones que el proyecto
arrastra desde la etapa 1 — separador `;` para campos y `|` para listas dentro
de un campo — de modo que quien ya leyó el catálogo puede leer este archivo
con el mismo código. La exportación recorre el stream con recursión de cola
(un acumulador en lugar de un contador mutable) y devuelve cuántas
combinaciones escribió, para que `main.rkt` pueda reportarlo.

La ruta del archivo es un parámetro de `exportar-combinaciones`; el punto de
entrada decide dónde escribir (por ejemplo `output/combinaciones.csv`).

## Decisiones de diseño

- **Contratos en lugar de Typed Racket.** Se usó `struct/contract` porque
  valida cada campo en el momento de construir el dato y reporta el campo
  incorrecto. Así los errores aparecen cerca de la lectura y no más adelante
  en la generación.
- **Lectura por recursión, sin mutación.** Los divisores de texto y el lector
  de preferencias usan bucles recursivos con acumuladores, de acuerdo con el
  requisito de inmutabilidad.
- **Carrera por curso derivada de la guía de horarios.** Los cursos de
  Ingeniería en Computadores y los de Enseñanza de la Matemática se separan con
  las tablas de la hoja "Cursos CE" de `data/Grupos CE.xlsx` y la guía de la
  licenciatura. La columna `carrera` se agregó a `data/cursos.csv` y el módulo
  de C la exporta en el catálogo.
- **Validación de preferencias separada de la lectura.** `leer-preferencias`
  solo interpreta el archivo; `validar-preferencias` devuelve mensajes. Así
  `main.rkt` puede mostrar advertencias sin detener la carga.

## Caso límite encontrado

Al leer el catálogo de C, el lector se rompía porque el módulo de C no exportaba
la columna `carrera`; la salida tenía 11 campos y el lector esperaba 12. Se
corrigió agregando la columna en `data/cursos.csv` y en `src/salida.c`, y el
lector ahora rechaza explícitamente las líneas con número de campos incorrecto
con un mensaje que indica la causa.
