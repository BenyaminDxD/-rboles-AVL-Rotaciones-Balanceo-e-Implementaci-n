# =====================================================================
#  avl_python.py  —  AVL en Python 3.12+
#  SIS210 Algoritmos y Estructuras de Datos · UNAP 2026-II · Semana 10
#
#  Uso:
#    python avl_python.py              -> menú interactivo SIGA-UNAP-AVL (Act. 3)
#    python avl_python.py experimento  -> tabla BST vs AVL (Act. 2B)
# =====================================================================
import math
import random
import sys
from dataclasses import dataclass
from time import perf_counter


# ---------------------------------------------------------------------
#  Nodo y funciones AVL (equivalentes a avl.h)
# ---------------------------------------------------------------------
class Nodo:
    def __init__(self, clave, valor=None):
        self.clave = clave
        self.valor = valor
        self.izq = self.der = None
        self.altura = 1


def height(n):
    """Altura almacenada del nodo (0 si es None)."""
    return n.altura if n else 0


def fb(n):
    """Factor de balance = h(der) - h(izq)."""
    return height(n.der) - height(n.izq) if n else 0


def actualizar_altura(n):
    n.altura = 1 + max(height(n.izq), height(n.der))


def rotar_der(z):
    """Rotación simple a la derecha (caso LL). Devuelve la nueva raíz."""
    y = z.izq
    z.izq = y.der
    y.der = z
    actualizar_altura(z)   # z quedó abajo: se actualiza primero
    actualizar_altura(y)
    return y


def rotar_izq(z):
    """Rotación simple a la izquierda (caso RR). Devuelve la nueva raíz."""
    y = z.der
    z.der = y.izq
    y.izq = z
    actualizar_altura(z)
    actualizar_altura(y)
    return y


def balancear(n):
    """Corrige los 4 casos LL, LR, RR, RL. Devuelve la raíz del subárbol."""
    actualizar_altura(n)
    f = fb(n)
    if f < -1:                       # pesado a la izquierda
        if fb(n.izq) > 0:            # LR
            n.izq = rotar_izq(n.izq)
        return rotar_der(n)          # LL
    if f > 1:                        # pesado a la derecha
        if fb(n.der) < 0:            # RL
            n.der = rotar_der(n.der)
        return rotar_izq(n)          # RR
    return n


def insertar_avl(raiz, clave, valor=None):
    """Inserción BST + balanceo al retornar. Clave repetida: actualiza valor."""
    if raiz is None:
        return Nodo(clave, valor)
    if clave < raiz.clave:
        raiz.izq = insertar_avl(raiz.izq, clave, valor)
    elif clave > raiz.clave:
        raiz.der = insertar_avl(raiz.der, clave, valor)
    else:
        raiz.valor = valor
        return raiz
    return balancear(raiz)


def buscar(raiz, clave):
    """Búsqueda iterativa O(log n). Devuelve el nodo o None."""
    while raiz is not None:
        if clave == raiz.clave:
            return raiz
        raiz = raiz.izq if clave < raiz.clave else raiz.der
    return None


def minimo(n):
    while n.izq:
        n = n.izq
    return n


def eliminar_avl(raiz, clave):
    """Eliminación AVL (3 casos BST + balanceo). Devuelve la nueva raíz."""
    if raiz is None:
        return None
    if clave < raiz.clave:
        raiz.izq = eliminar_avl(raiz.izq, clave)
    elif clave > raiz.clave:
        raiz.der = eliminar_avl(raiz.der, clave)
    else:
        if raiz.izq is None:          # hoja o solo hijo derecho
            return raiz.der
        if raiz.der is None:          # solo hijo izquierdo
            return raiz.izq
        suc = minimo(raiz.der)        # dos hijos: sucesor inorden
        raiz.clave, raiz.valor = suc.clave, suc.valor
        raiz.der = eliminar_avl(raiz.der, suc.clave)
    return balancear(raiz)


def inorden(n):
    """Generador del recorrido inorden (claves en orden ascendente)."""
    pila = []
    while pila or n:
        while n:
            pila.append(n)
            n = n.izq
        n = pila.pop()
        yield n
        n = n.der


# ---------------------------------------------------------------------
#  ACTIVIDAD 2B — Experimento BST simple vs AVL
# ---------------------------------------------------------------------
def insertar_bst(raiz, clave):        # BST simple sin balanceo
    if raiz is None:
        return Nodo(clave)
    if clave < raiz.clave:
        raiz.izq = insertar_bst(raiz.izq, clave)
    elif clave > raiz.clave:
        raiz.der = insertar_bst(raiz.der, clave)
    return raiz


def altura_arbol(n):
    if n is None:
        return 0
    return 1 + max(altura_arbol(n.izq), altura_arbol(n.der))


def experimento(n, orden='random'):
    claves = list(range(1, n + 1))
    if orden == 'random':
        random.shuffle(claves)

    # El BST ordenado degenera en lista: recursión de profundidad n
    sys.setrecursionlimit(max(sys.getrecursionlimit(), 2 * n + 1000))

    # BST simple
    t0 = perf_counter()
    raiz_bst = None
    for c in claves:
        raiz_bst = insertar_bst(raiz_bst, c)
    t_bst = perf_counter() - t0
    h_bst = altura_arbol(raiz_bst)

    # AVL
    t0 = perf_counter()
    raiz_avl = None
    for c in claves:
        raiz_avl = insertar_avl(raiz_avl, c)
    t_avl = perf_counter() - t0
    h_avl = raiz_avl.altura if raiz_avl else 0
    assert h_avl == altura_arbol(raiz_avl), "altura almacenada inconsistente"

    limite_teorico = int(1.44 * math.log2(n + 1))
    return h_bst, h_avl, limite_teorico, t_bst, t_avl


def correr_experimento(semilla=2026):
    random.seed(semilla)              # resultados reproducibles
    tamanos = [100, 500, 1000, 5000, 10000]
    filas = []
    print(f"{'n':>8} | {'BST (ord)':>10} | {'AVL (ord)':>10} | {'Teorico':>10}"
          f" | {'BST (rnd)':>10} | {'AVL (rnd)':>10}")
    print('-' * 73)
    for n in tamanos:
        h_bst_o, h_avl_o, teo, tbo, tao = experimento(n, 'ordered')
        h_bst_r, h_avl_r, _, tbr, tar = experimento(n, 'random')
        filas.append((n, tbo, tao, tbr, tar))
        print(f"{n:>8} | {h_bst_o:>10} | {h_avl_o:>10} | {teo:>10}"
              f" | {h_bst_r:>10} | {h_avl_r:>10}")

    print("\nTiempo total de insercion (ms)")
    print(f"{'n':>8} | {'BST (ord)':>10} | {'AVL (ord)':>10}"
          f" | {'BST (rnd)':>10} | {'AVL (rnd)':>10}")
    print('-' * 60)
    for n, tbo, tao, tbr, tar in filas:
        print(f"{n:>8} | {tbo*1e3:>10.1f} | {tao*1e3:>10.1f}"
              f" | {tbr*1e3:>10.1f} | {tar*1e3:>10.1f}")


# ---------------------------------------------------------------------
#  ACTIVIDAD 3 — Sistema SIGA-UNAP-AVL
# ---------------------------------------------------------------------
@dataclass
class EstudianteUNAP:
    codigo: str
    nombre: str
    escuela: str
    promedio: float


class SIGA_AVL:
    def __init__(self):
        self.raiz = None
        self.total = 0

    def matricular(self, est: EstudianteUNAP):
        t0 = perf_counter()
        existia = buscar(self.raiz, est.codigo) is not None
        # insertar est con est.codigo como clave
        self.raiz = insertar_avl(self.raiz, est.codigo, est)
        if not existia:
            self.total += 1
        t1 = perf_counter()
        accion = 'Actualizado' if existia else 'Matriculado'
        print(f'  {accion} {est.nombre} en {(t1 - t0) * 1e6:.2f} µs')

    def buscar_estudiante(self, codigo: str):
        t0 = perf_counter()
        nodo = buscar(self.raiz, codigo)
        t1 = perf_counter()
        if nodo:
            e = nodo.valor
            print(f'  Encontrado: {e.codigo} | {e.nombre} | {e.escuela} | '
                  f'{e.promedio:.1f}  ({(t1 - t0) * 1e6:.2f} µs)')
        else:
            print(f'  Código {codigo} no encontrado ({(t1 - t0) * 1e6:.2f} µs)')
        return nodo

    def listar_ordenado(self):
        # Recorrido inorden del AVL => lista ordenada por código
        if self.raiz is None:
            print('  (no hay estudiantes)')
            return []
        lista = [n.valor for n in inorden(self.raiz)]
        print(f"  {'#':>3}  {'Código':<12}{'Nombre':<20}{'Escuela':<12}{'Prom.':>6}")
        print('  ' + '-' * 55)
        for i, e in enumerate(lista, 1):
            print(f'  {i:>3}  {e.codigo:<12}{e.nombre:<20}{e.escuela:<12}{e.promedio:>6.1f}')
        return lista

    def estadisticas(self):
        h = self.raiz.altura if self.raiz else 0
        teo = int(1.44 * math.log2(self.total + 1)) if self.total > 0 else 0
        h_min = math.ceil(math.log2(self.total + 1)) if self.total > 0 else 0
        print(f'  Total estudiantes: {self.total}')
        print(f'  Altura AVL actual: {h}')
        print(f'  Altura mínima posible: {h_min}')
        print(f'  Altura teórica max: {teo}')
        print(f'  Factor de balance raíz: {fb(self.raiz)}')
        print(f'  Eficiencia: {"ÓPTIMO" if h <= teo else "SUBÓPTIMO"}')


def leer_promedio():
    while True:
        txt = input('  Promedio: ').strip().replace(',', '.')
        try:
            p = float(txt)
            if 0 <= p <= 20:
                return p
        except ValueError:
            pass
        print('  Promedio inválido (0-20). Intente otra vez.')


def menu():
    siga = SIGA_AVL()
    # Cargar 10 estudiantes de muestra
    muestra = [
        EstudianteUNAP('2026100001', 'Ana Quispe', 'Sistemas', 15.5),
        EstudianteUNAP('2026100002', 'Luis Mamani', 'Sistemas', 14.0),
        EstudianteUNAP('2026100003', 'Rosa Condori', 'IA&CD', 17.2),
        EstudianteUNAP('2026100004', 'Ivan Huanca', 'Sistemas', 13.8),
        EstudianteUNAP('2026100005', 'Maria Apaza', 'IA&CD', 16.9),
        EstudianteUNAP('2026100006', 'Carlos Tito', 'Sistemas', 12.1),
        EstudianteUNAP('2026100007', 'Lucia Ramos', 'IA&CD', 18.0),
        EstudianteUNAP('2026100008', 'Pedro Vargas', 'Sistemas', 15.0),
        EstudianteUNAP('2026100009', 'Sofia Luna', 'IA&CD', 16.4),
        EstudianteUNAP('2026100010', 'Juan Puma', 'Sistemas', 14.7),
    ]
    print('Cargando 10 estudiantes...')
    for e in muestra:
        siga.matricular(e)

    while True:
        print('\n=== SIGA-UNAP-AVL ===')
        print('1. Matricular nuevo estudiante')
        print('2. Buscar por código')
        print('3. Listar todos (inorden)')
        print('4. Estadísticas del árbol')
        print('5. Salir')
        try:
            op = input('Opción: ').strip()
        except EOFError:
            break
        if op == '1':
            cod = input('  Código: ').strip()
            if not cod:
                print('  El código no puede estar vacío.')
                continue
            nom = input('  Nombre: ').strip()
            esc = input('  Escuela: ').strip()
            prom = leer_promedio()
            siga.matricular(EstudianteUNAP(cod, nom, esc, prom))
        elif op == '2':
            siga.buscar_estudiante(input('  Código: ').strip())
        elif op == '3':
            siga.listar_ordenado()
        elif op == '4':
            siga.estadisticas()
        elif op == '5':
            print('Hasta luego.')
            break
        else:
            print('  Opción no válida.')


if __name__ == '__main__':
    if len(sys.argv) > 1 and sys.argv[1].lower().startswith('exp'):
        correr_experimento()
    else:
        menu()
