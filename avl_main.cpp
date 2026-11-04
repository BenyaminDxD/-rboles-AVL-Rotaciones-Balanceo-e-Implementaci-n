// =====================================================================
//  avl_main.cpp  —  Casos de prueba P1–P6 del AVL
//  SIS210 · UNAP 2026-II · Semana 10
//  Compilar:  g++ -std=c++17 -Wall -o avl avl_main.cpp
//  Ejecutar:  ./avl        (Windows: avl.exe)
//
//  Nota: en la guía los mensajes de assert/cout usan comillas simples
//  ('texto'), que en C++ son constantes multicarácter (un int), no
//  cadenas. Aquí se cambiaron a comillas dobles para que compile sin
//  advertencias y el texto se imprima correctamente.
// =====================================================================
#include <iostream>
#include <cassert>
#include <string>
#include "avl.h"

// Función auxiliar: retorna altura del árbol
template<typename K, typename V>
int h(const std::unique_ptr<NodoBST<K,V>>& r) { return r ? r->altura : 0; }

// Imprime el árbol "acostado" (raíz a la izquierda, hijo derecho arriba)
template<typename K, typename V>
void imprimir(const std::unique_ptr<NodoBST<K,V>>& n,
              const std::string& pref = "", bool esDer = false, bool raiz = true) {
    if (!n) return;
    imprimir(n->der, pref + (raiz ? "" : (esDer ? "    " : "|   ")), true, false);
    std::cout << pref << (raiz ? "" : (esDer ? "/-- " : "\\-- "))
              << n->clave << " (h=" << n->altura
              << ", fb=" << factor_balance(n) << ")\n";
    imprimir(n->izq, pref + (raiz ? "" : (esDer ? "|   " : "    ")), false, false);
}

template<typename K, typename V>
void imprimir_inorden(const std::unique_ptr<NodoBST<K,V>>& r) {
    std::cout << "  inorden: ";
    inorden(r, [](const NodoBST<K,V>& n) { std::cout << n.clave << ' '; });
    std::cout << '\n';
}

// ---------------------------------------------------------------- P1
void test_P1_insercion_simple() {
    // Insertar 3 claves ordenadas — en BST sería h=3; en AVL h=2
    std::unique_ptr<NodoBST<int,int>> r = nullptr;
    g_rotaciones = 0;
    insertar_avl(r, 10, 10);
    insertar_avl(r, 20, 20);
    insertar_avl(r, 30, 30);              // provoca caso RR -> rotar_izquierda
    assert(h(r) == 2 && "P1: altura debe ser 2 tras RR rotation");
    assert(r->clave == 20 && "P1: raiz debe ser 20");
    assert(verificar_avl(r) == h(r) && "P1: arbol AVL valido");
    std::cout << "[P1 OK] insercion_simple  (rotaciones simples: "
              << g_rotaciones << ", caso RR)\n";
    imprimir(r);
}

// ---------------------------------------------------------------- P2
void test_P2_caso_LR() {
    std::unique_ptr<NodoBST<int,int>> r = nullptr;
    g_rotaciones = 0;
    insertar_avl(r, 30, 30);
    insertar_avl(r, 10, 10);
    insertar_avl(r, 20, 20);              // provoca rotacion LR
    assert(r->clave == 20 && "P2: raiz debe ser 20");
    assert(h(r) == 2 && "P2: altura debe ser 2");
    assert(verificar_avl(r) == h(r) && "P2: arbol AVL valido");
    std::cout << "[P2 OK] caso_LR  (rotaciones simples: "
              << g_rotaciones << " = 1 doble LR)\n";
    imprimir(r);
}

// ---------------------------------------------------------------- P3
void test_P3_insercion_ordenada_grande() {
    // Insertar 1..15 en orden — BST degeneraría h=15
    std::unique_ptr<NodoBST<int,int>> r = nullptr;
    g_rotaciones = 0;
    for (int i = 1; i <= 15; i++) insertar_avl(r, i, i);
    assert(h(r) <= 4 && "P3: altura debe ser <= 4 para 15 nodos AVL");
    assert(buscar(r, 8) && "P3: nodo 8 debe existir");
    assert(verificar_avl(r) == h(r) && "P3: arbol AVL valido");
    std::cout << "[P3 OK] insercion_ordenada_grande h=" << h(r)
              << "  (rotaciones simples: " << g_rotaciones << ")\n";
    imprimir(r);
}

// ---------------------------------------------------------------- P4
void test_P4_eliminar_hoja() {
    std::unique_ptr<NodoBST<int,int>> r = nullptr;
    for (int x : {20, 10, 30}) insertar_avl(r, x, x);
    eliminar_avl(r, 10);                  // hoja izquierda
    assert(!buscar(r, 10) && "P4: nodo 10 debe haberse eliminado");
    assert(h(r) <= 2 && "P4: arbol sigue balanceado");
    assert(verificar_avl(r) == h(r) && "P4: arbol AVL valido");
    std::cout << "[P4 OK] eliminar_hoja\n";
    imprimir(r);
}

// ---------------------------------------------------------------- P5
void test_P5_eliminar_raiz_dos_hijos() {
    std::unique_ptr<NodoBST<int,int>> r = nullptr;
    for (int x : {20, 10, 30, 25, 35}) insertar_avl(r, x, x);
    int raiz_orig = r->clave;
    eliminar_avl(r, 20);                  // nodo raiz con 2 hijos
    assert(!buscar(r, 20) && "P5: nodo 20 eliminado");
    assert(h(r) <= 3 && "P5: arbol sigue balanceado");
    assert(verificar_avl(r) == h(r) && "P5: arbol AVL valido");
    std::cout << "[P5 OK] eliminar_raiz_dos_hijos  (raiz " << raiz_orig
              << " -> " << r->clave << ", sucesor inorden)\n";
    imprimir(r);
}

// ---------------------------------------------------------------- P6
void test_P6_eliminar_con_rebalanceo() {
    // Insertar 1..7, eliminar 1 y verificar balanceo
    std::unique_ptr<NodoBST<int,int>> r = nullptr;
    for (int i = 1; i <= 7; i++) insertar_avl(r, i, i);
    int h_antes = h(r);
    eliminar_avl(r, 1);
    // Verificar que el árbol sigue siendo AVL
    assert(h(r) <= h_antes && "P6: altura no aumento tras eliminacion");
    assert(verificar_avl(r) == h(r) && "P6: arbol AVL valido");
    std::cout << "[P6 OK] eliminar_con_rebalanceo  (h antes=" << h_antes
              << ", h despues=" << h(r) << ")\n";
    imprimir(r);
}

// ------------------------------------------- P7 (extra): estrés
// Inserta y elimina 2000 claves pseudoaleatorias verificando el AVL
// después de cada operación. Fuerza también los casos RL y LL.
void test_P7_estres() {
    std::unique_ptr<NodoBST<int,int>> r = nullptr;
    unsigned s = 12345;
    auto rnd = [&s]() { s = s * 1103515245u + 12345u; return int((s >> 8) % 5000); };
    int ops = 0;
    for (int i = 0; i < 2000; i++) {
        insertar_avl(r, rnd(), i);
        assert(verificar_avl(r) == h(r) && "P7: AVL valido tras insertar");
        ++ops;
    }
    for (int i = 0; i < 2000; i++) {
        eliminar_avl(r, rnd());
        assert(verificar_avl(r) == h(r) && "P7: AVL valido tras eliminar");
        ++ops;
    }
    std::cout << "[P7 OK] estres: " << ops
              << " operaciones, invariante AVL verificada en cada paso\n";
}

int main() {
    test_P1_insercion_simple();      std::cout << '\n';
    test_P2_caso_LR();               std::cout << '\n';
    test_P3_insercion_ordenada_grande(); std::cout << '\n';
    test_P4_eliminar_hoja();         std::cout << '\n';
    test_P5_eliminar_raiz_dos_hijos(); std::cout << '\n';
    test_P6_eliminar_con_rebalanceo(); std::cout << '\n';
    test_P7_estres();
    std::cout << "=== Todos los casos P1-P6 pasaron ===\n";
    return 0;
}
