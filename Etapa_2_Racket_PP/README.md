## Persona 2: Motor de Combinaciones y Streams (Ian Alejandro Bonilla Mena)

## Arquitectura Funcional del Proyecto

Para esta etapa en Racket, el sistema se diseñó bajo un modelo de tubería de datos (pipeline) puramente funcional, asegurando la inmutabilidad y la ausencia de efectos secundarios (sin uso de set! ni variables globales reasignadas). La arquitectura divide la solución en tres módulos aislados que se conectan a través de main.rkt:

Entrada y Tipado: Un módulo que lee el archivo serializado de la Etapa 1 y lo transforma en estructuras de Racket fuertemente tipadas.

Generación (Este componente): Un motor central, implementado en generador.rkt, que recibe el catálogo limpio y produce un flujo de datos (stream) continuo con las combinaciones de horarios válidos.

Filtrado y Salida: Un módulo final que consume el stream, descarta las combinaciones fuera del rango de créditos y serializa el resultado para Prolog.

El estado del programa nunca se modifica; en su lugar, se transfiere mediante parámetros en funciones recursivas a lo largo del flujo de ejecución.

## 1. Algoritmo de Generación mediante Streams
El reto principal consistía en evitar la explosión combinatoria. Generar el producto cartesiano de todos los grupos de todos los cursos almacenando listas completas en memoria saturaría el sistema rápidamente. Para solucionarlo, se implementó un motor de evaluación perezosa (lazy evaluation) utilizando la librería racket/stream.

Poda algorítmica temprana (Pruning): El algoritmo recursivo evalúa un curso a la vez. Mediante el uso de foldr, el sistema intenta agregar cada grupo a la combinacion-actual. Si la función choca-con-horario? detecta un conflicto, esa rama de recursión devuelve el stream acumulado sin procesar los cursos restantes, podando inmediatamente millones de combinaciones inválidas antes de que se calculen.

Flujo bajo demanda: Utilizando stream-append y la naturaleza recursiva del algoritmo, el motor no devuelve una lista, sino una promesa de cálculo. Las combinaciones válidas solo se generan en memoria a medida que el módulo de filtrado (Persona 3) o la exportación las va solicitando.

## 2. Casos Límite
Caso límite real: Se detectó un problema de rendimiento exponencial cuando un curso poseía múltiples grupos con horarios complejos (ej. clases teóricas de 2 días y laboratorios de 1 día) y el algoritmo intentaba validarlos contra un horario que ya tenía 4 o 5 cursos matriculados. Realizar iteraciones tradicionales sobre todas esas listas anidadas generaba cuellos de botella temporales.

Solución técnica: Se reestructuró la validación matemática de choques heredada de C, transformándola en una composición de funciones de orden superior (ormap). Al anidar llamadas a ormap (para iterar sobre los horarios del grupo nuevo, y a su vez sobre los horarios de los grupos ya matriculados), el lenguaje aplica una evaluación de cortocircuito (short-circuit) de forma nativa. En el instante exacto en que la expresión lógica de superposición de horas (que valida la hora de inicio y fin) detecta una colisión en un solo día, el ormap interno retorna #t y detiene inmediatamente la evaluación de los días restantes de la semana y de los otros cursos, optimizando drásticamente el tiempo de procesamiento de cada rama del stream.

## 3. Decisiones de Diseño Funcional
Ausencia de estado global en el horario: A diferencia de una matriz semanal o un arreglo global donde se marcan las horas ocupadas, se decidió representar el "horario actual" simplemente como una lista enlazada de las estructuras grupo seleccionadas en la rama de recursión vigente. Esto garantiza la pureza de la función de generación.

Aislamiento de la lógica de evaluación: Se separó estrictamente la función chocan-horarios? (que evalúa dos bloques individuales) de la función choca-con-horario? (que evalúa listas de grupos). Este diseño modular permite que la lógica base sea testeada de forma aislada e independiente de los datos temporales (mock data) o de las estructuras definitivas inyectadas por el módulo de lectura de la Persona 1.