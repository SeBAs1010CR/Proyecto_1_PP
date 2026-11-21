# 📂 Proyecto 1 - Etapa 3: Sistema de Validación de Políticas Académicas

## 🤖 Rol: Persona 2 (Motor de Reglas - Prolog)
El módulo `politicas.pl` actúa como el núcleo de validación lógica del sistema. Su objetivo es evaluar de forma estricta, eficiente y modular las combinaciones de cursos generadas, asegurando el cumplimiento de las políticas institucionales antes de emitir un veredicto definitivo.

---

## 📋 Reglas Académicas Validadas
El motor evalúa de manera secuencial tres restricciones institucionales clave:
1. **Regla de Incompatibilidad:** Verifica que la combinación no incluya simultáneamente ambos cursos de un par incompatible definido (relación simétrica, implementado mediante negación por falla `\+`).
2. **Límite por Área:** Comprueba que la cantidad de cursos por área específica no rebase los límites máximos permitidos (por ejemplo: Programación $\le 2$, Matemática $\le 2$, Física $\le 1$).
3. **Balance entre Carreras:** Valida que ninguna carrera acapare un porcentaje mayor al permitido del total de créditos de la combinación (configurado por defecto en un máximo del $70\%$ para Ingeniería en Computadores).

---

## 🔌 Interfaz y Contrato de la API (Predicado Principal)
El módulo exporta el predicado principal de evaluación para integrarse con el resto del pipeline del proyecto:

    evaluar_combinacion(+Combinacion, -Veredicto, -Razon)

* **`+Combinacion`**: Lista de términos de cursos con la estructura exacta esperada:
    
    curso(Codigo, Grupo, Creditos, Area, Carrera)
    
  *Ejemplo:* `curso('CE1101', 1, 4, 'Programacion', 'Ingenieria en Computadores')`
* **`-Veredicto`**: Toma el valor de `cumple` o `no_cumple`.
* **`-Razon`**: Devuelve una cadena de texto descriptiva indicando el estado o la causa exacta del rechazo.

---

## 📊 Posibles Resultados y Razones de Retorno

| Veredicto | Razón devuelta (`Razon`) | Descripción |
| :--- | :--- | :--- |
| `cumple` | `'Ninguna'` | La combinación pasa exitosamente todas las políticas académicas. |
| `no_cumple` | `'Violacion de regla de incompatibilidad'` | Se detectaron dos cursos incompatibles dentro de la combinación. |
| `no_cumple` | `'Exceso de cursos permitidos por area'` | Se superó el límite de cursos permitidos para una o más áreas académicas. |
| `no_cumple` | `'Incumplimiento de balance de carreras'` | Una carrera excede el porcentaje máximo de créditos permitidos. |

---

## 🚀 Instrucciones de Uso y Pruebas en SWI-Prolog

1. Abra una terminal de PowerShell y navegue hasta la ruta del módulo:
    
    cd Etapa3/src
    
2. Inicie el entorno de SWI-Prolog (utilizando la ruta del ejecutable si es necesario):
    
    & "C:\Program Files\swipl\bin\swipl.exe"
    
3. Cargue el módulo de políticas:
    
    ?- [politicas].
    
4. Ejecute una consulta de prueba en la consola:
    
    ?- evaluar_combinacion([curso('CE1101', 1, 6, 'Programacion', 'Ingenieria en Computadores'), curso('MA1404', 1, 4, 'Matematica', 'Ingenieria en Produccion Industrial')], Veredicto, Razon).

---

## 🧭 Rol: Persona 3 (Control de Flujo y Exportación)

### Arquitectura de la etapa

```
Etapa3/
├── main.pl              Punto de entrada (se integra con la carga de datos)
├── src/
│   ├── politicas.pl     Motor de reglas de política académica (Persona 2)
│   ├── evaluador.pl     Capa alternativa de veredicto con orden de reglas configurable
│   ├── recorrido.pl     Recorrido por backtracking de todas las combinaciones
│   └── exportador.pl    Exportación del veredicto y la razón a CSV
└── tests/               Pruebas plunit de cada módulo
```

Flujo: la carga de datos aserta un hecho `recorrido:combinacion(Id, Cursos)`
por cada combinación del archivo de Racket → `combinacion_evaluada/3` las
evalúa una a una por backtracking contra `evaluar_combinacion/3` →
`exportar_resultados/2` escribe el archivo final para la etapa 4.

### Recorrido por backtracking y modos de consulta

`combinacion_evaluada(?Id, ?Veredicto, ?Razon)` (en `recorrido.pl`) es el punto
de backtracking del sistema: cada re-satisfacción entrega la siguiente
combinación con su veredicto. Es reversible, por lo que admite múltiples modos
de consulta:

```prolog
?- combinacion_evaluada(Id, Veredicto, Razon).    % enumera todas por backtracking
?- combinacion_evaluada(c2, Veredicto, Razon).    % evalúa una combinación puntual
?- combinacion_evaluada(Id, cumple, _).           % solo las que cumplen
?- combinacion_evaluada(Id, _, 'Incumplimiento de balance de carreras').
                                                  % las rechazadas por una razón dada
?- evaluar_catalogo(Resultados).                  % recorrido exhaustivo con findall
```

### Caso límite real encontrado

Al consultar el motor con la razón ya instanciada (por ejemplo "¿cuáles
combinaciones fallan por balance?"), la unificación de la cabeza descarta las
cláusulas anteriores de `evaluar_combinacion/3` sin ejecutar sus cortes, y una
combinación cuya primera violación es la incompatibilidad aparecía también como
rechazada por balance. La solución quedó en la capa de recorrido: se evalúa
primero con variables frescas (el modo natural del motor, donde los cortes sí
garantizan un único veredicto) y la unificación con los argumentos del
consultante se hace después. Así el predicado es reversible sin modificar el
motor de reglas.

### Formato de salida y justificación

`exportar_resultados(+Ruta, -Total)` escribe un CSV separado por `;`, una línea
por combinación, con los pares curso-grupo en el mismo formato de la etapa 2:

```
combinacion;cursos;veredicto;razon
c1;CE1101-G3|EM5001-G1;cumple;Ninguna
c2;CE1101-G3|CE1103-G2;no_cumple;Violacion de regla de incompatibilidad
```

Justificación ligada a la decisión de diseño: la etapa 4 (Java) consume cada
combinación como una unidad con su veredicto, así que una línea por combinación
se traduce en un objeto por línea con un `split(";")`, sin reconstruir estado
entre líneas. Se mantienen las convenciones que el proyecto arrastra desde la
etapa 1 (`;` para campos, `|` para listas dentro de un campo) y la razón viaja
con el texto exacto que produce el motor de reglas, para mostrarla al usuario
final sin tablas de traducción intermedias.

### Pruebas de la etapa

Desde la raíz del repositorio:

```
swipl -g run_tests -t halt Etapa3/tests/evaluador_tests.pl
swipl -g run_tests -t halt Etapa3/tests/recorrido_tests.pl
swipl -g run_tests -t halt Etapa3/tests/exportador_tests.pl
```

Las pruebas del recorrido y del exportador evalúan combinaciones de ejemplo
contra las reglas reales de `politicas.pl`, cubriendo un caso por regla, el
caso que cumple y los modos de consulta del predicado reversible.