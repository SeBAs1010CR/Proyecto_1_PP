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