# Árboles AVL: rotaciones, balanceo e implementación

Práctica de la **Semana 10** del curso **SIS210 Algoritmos y Estructuras de Datos**
(Universidad Nacional del Altiplano, Escuela Profesional de Ingeniería de Sistemas, 2026-II).

Contiene un árbol AVL genérico en **C++17** con inserción, eliminación y las cuatro rotaciones; un
experimento en **Python** que compara la altura de un BST simple con la de un AVL; el sistema
interactivo **SIGA-UNAP-AVL**; y un **visualizador web** que anima cada paso del algoritmo.

## Contenido

```
arboles-avl-sis210/
├── cpp/
│   ├── avl.h            # AVL genérico: rotaciones, balancear, insertar, eliminar, buscar, verificar
│   └── avl_main.cpp     # Casos de prueba P1–P6 (+ P7: 4000 operaciones aleatorias)
├── python/
│   └── avl_python.py    # Experimento BST vs AVL (2B) + menú SIGA-UNAP-AVL (Act. 3)
├── docs/
│   └── index.html       # Visualizador interactivo (se publica con GitHub Pages)
├── informe/
│   ├── informe_s10_avl.tex   # Informe en LaTeX (pdfLaTeX / Overleaf)
│   ├── informe_s10_avl.pdf   # Informe compilado
│   └── Practica_S10_AVL.docx # Versión Word
├── RESPUESTAS.md        # Respuestas a las preguntas de análisis
├── Makefile
└── .github/workflows/ci.yml  # Compila y prueba todo en cada push
```

## Cómo ejecutar

### C++ (Actividades 1 y 2A)

```bash
g++ -std=c++17 -Wall -o avl cpp/avl_main.cpp
./avl            # en Windows: avl.exe
```

Salida esperada (resumida):

```
[P1 OK] insercion_simple  (rotaciones simples: 1, caso RR)
[P2 OK] caso_LR  (rotaciones simples: 2 = 1 doble LR)
[P3 OK] insercion_ordenada_grande h=4  (rotaciones simples: 11)
[P4 OK] eliminar_hoja
[P5 OK] eliminar_raiz_dos_hijos  (raiz 20 -> 25, sucesor inorden)
[P6 OK] eliminar_con_rebalanceo  (h antes=3, h despues=3)
[P7 OK] estres: 4000 operaciones, invariante AVL verificada en cada paso
=== Todos los casos P1-P6 pasaron ===
```

### Python 3.12+ (Actividades 2B y 3)

```bash
python python/avl_python.py experimento   # tabla BST vs AVL
python python/avl_python.py               # menú SIGA-UNAP-AVL
```

### Con `make`

```bash
make test         # compila y ejecuta las pruebas C++
make experimento  # corre el experimento en Python
make siga         # abre el menú interactivo
make clean
```

### Visualizador

Abre `docs/index.html` en el navegador. Si activas **GitHub Pages** (ver abajo), queda en
`https://<tu-usuario>.github.io/arboles-avl-sis210/`.

Muestra el árbol animado, la línea de código C++ o Python que se está ejecutando, la pila de
llamadas y una explicación de cada paso. Incluye los casos P1–P6, la comparación BST vs AVL y el
experimento 2B.

## Resultados del experimento (Actividad 2B)

Altura del árbol al insertar *n* claves (semilla `2026`):

|      n | BST ordenado | AVL ordenado | Límite 1.44·log₂(n+1) | BST aleatorio | AVL aleatorio |
|-------:|-------------:|-------------:|----------------------:|--------------:|--------------:|
|    100 |          100 |            7 |                     9 |            12 |             8 |
|    500 |          500 |            9 |                    12 |            18 |            11 |
|  1 000 |        1 000 |           10 |                    14 |            20 |            12 |
|  5 000 |        5 000 |           13 |                    17 |            27 |            15 |
| 10 000 |       10 000 |           14 |                    19 |            29 |            16 |

Con claves ordenadas, el BST degenera en una lista (*h = n*). El AVL se mantiene en altura
logarítmica y, con *n* = 10 000, alcanza la altura mínima posible (14). 

## Notas de implementación

- `fb = h(der) − h(izq)`. Si `fb < −1`, el nodo está cargado a la izquierda (casos LL/LR); si
  `fb > +1`, a la derecha (casos RR/RL).
- Las rotaciones mueven la propiedad de los `std::unique_ptr` con `std::move`. Se actualiza
  primero la altura del nodo que baja y luego la del que sube.
- `verificar_avl()` comprueba el orden BST, las alturas y que |fb| ≤ 1 en todo el árbol. Todas
  las pruebas la usan.
- En la guía, los mensajes de `assert` y `cout` usaban comillas simples (`'texto'`), que en C++
  son constantes multicarácter y no cadenas. Aquí se cambiaron a comillas dobles.

## Publicar el visualizador con GitHub Pages

1. En el repositorio: **Settings → Pages**.
2. En *Build and deployment*, elige **Deploy from a branch**, rama `main` y carpeta `/docs`.
3. Guarda. A los pocos minutos aparece el enlace.

## Autor

- Nombre: Benjamin Jorge Chuquimia Arroyo · Código: 250273
- Docente: Dr. Aldo Hernán Zanabria Gálvez
