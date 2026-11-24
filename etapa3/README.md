# CEmestre - Etapa 3 (Prolog)

Parte de la Persona 1: carga de datos y definición de áreas y pares
incompatibles. Las reglas de política (Persona 2) y la exportación del
veredicto (Persona 3) no están en esta carpeta todavía.

## Estructura

```
etapa3/
├── src/carga.pl              Carga del catálogo, combinaciones y políticas
└── data/
    ├── politicas.pl          Pares de cursos incompatibles (hechos)
    └── combinaciones_ejemplo.txt  Entrada de prueba (una combinación por línea)
```

## Carga de datos

```prolog
?- consult('src/carga').
?- cargar_catalogo('../output/catalogo_salida.csv').
?- cargar_combinaciones('data/combinaciones_ejemplo.txt').
?- cargar_politicas('data/politicas.pl').
```

- `cargar_catalogo/1` deja un hecho `curso(Codigo, Creditos, Carrera, Area)`
  por curso. Si el curso aparece en varias líneas (un registro por grupo),
  solo se guarda una vez.
- `cargar_combinaciones/1` deja `combinacion(Id, [Codigo, ...])`, con `Id`
  desde 1. Formato del archivo: una combinación por línea, códigos separados
  por coma.
- `cargar_politicas/1` deja `incompatible(CodigoA, CodigoB)` leyendo los
  hechos de `data/politicas.pl`.

Las cargas reemplazan los hechos anteriores; no se acumulan.

## Área académica (asignada en C)

El campo `area` se calcula en `src/curso.c` a partir de los dos primeros
caracteres del código, y se exporta como última columna de
`output/catalogo_salida.csv`.

| Prefijo | Área |
|---------|------|
| CE, CS, EL, CA | Computación |
| MA, FI, QU, PI | Ciencias básicas |
| EM | Educación |
| CI, FH, SE, SO | Humanidades y artes |
| (otro) | Otras |

## Pares incompatibles

Definidos en `data/politicas.pl`:

- `MA0101` – `MA1102`: el curso de nivelación no se lleva junto con el primer cálculo.
- `MA1102` – `MA1103`: cálculos alternativos; el plan admite solo uno.

Son una propuesta y deben verificarse contra el plan de estudios oficial.
