// =====================================================================
//  avl.h  —  Árbol AVL genérico (C++17)
//  SIS210 Algoritmos y Estructuras de Datos · UNAP 2026-II · Semana 10
//  Compilar: g++ -std=c++17 -Wall -o avl avl_main.cpp
// =====================================================================
#pragma once
#include <memory>
#include <algorithm>
#include <string>
#include <utility>

// Contador global de rotaciones simples (sirve para responder P1-A/P1-B).
// Una rotación doble (LR o RL) cuenta como 2 rotaciones simples.
inline int g_rotaciones = 0;

template<typename K, typename V = K>
struct NodoBST {
    K clave; V valor;
    std::unique_ptr<NodoBST> izq, der;
    int altura;
    explicit NodoBST(K k, V v = V{})
        : clave(std::move(k)), valor(std::move(v)),
          izq(nullptr), der(nullptr), altura(1) {}
};

template<typename K, typename V>
int altura(const std::unique_ptr<NodoBST<K,V>>& n)
{ return n ? n->altura : 0; }

template<typename K, typename V>
int factor_balance(const std::unique_ptr<NodoBST<K,V>>& n)
{ return n ? altura(n->der) - altura(n->izq) : 0; }

template<typename K, typename V>
void actualizar_altura(std::unique_ptr<NodoBST<K,V>>& n)
{ if (n) n->altura = 1 + std::max(altura(n->izq), altura(n->der)); }

/* ---------------------------------------------------------------------
   TODO 1: rotación simple a la derecha (corrige el caso LL)

            z                y
           / \              / \
          y   T4    ==>    x   z
         / \                  / \
        x   T3              T3  T4
   --------------------------------------------------------------------- */
template<typename K, typename V>
void rotar_derecha(std::unique_ptr<NodoBST<K,V>>& z) {
    if (!z || !z->izq) return;
    std::unique_ptr<NodoBST<K,V>> y = std::move(z->izq); // y = z->izq
    z->izq = std::move(y->der);                         // z->izq = T3
    actualizar_altura(z);                               // z baja: primero
    y->der = std::move(z);                              // y->der = z
    actualizar_altura(y);                               // luego y
    z = std::move(y);                                   // y es la nueva raíz
    ++g_rotaciones;
}

/* ---------------------------------------------------------------------
   TODO 2: rotación simple a la izquierda (corrige el caso RR)

        z                    y
       / \                  / \
      T1  y      ==>       z   x
         / \              / \
        T2  x            T1 T2
   --------------------------------------------------------------------- */
template<typename K, typename V>
void rotar_izquierda(std::unique_ptr<NodoBST<K,V>>& z) {
    if (!z || !z->der) return;
    std::unique_ptr<NodoBST<K,V>> y = std::move(z->der); // y = z->der
    z->der = std::move(y->izq);                         // z->der = T2
    actualizar_altura(z);
    y->izq = std::move(z);                              // y->izq = z
    actualizar_altura(y);
    z = std::move(y);
    ++g_rotaciones;
}

// ---------------------------------------------------------------------
// TODO 3: balancear(n) — detecta y corrige los 4 casos
//   fb = h(der) - h(izq)
//   fb < -1  -> cargado a la izquierda
//        fb(izq) <= 0 : LL -> rotar_derecha(n)
//        fb(izq) >  0 : LR -> rotar_izquierda(n->izq); rotar_derecha(n)
//   fb > +1  -> cargado a la derecha
//        fb(der) >= 0 : RR -> rotar_izquierda(n)
//        fb(der) <  0 : RL -> rotar_derecha(n->der); rotar_izquierda(n)
//   (El "=" en fb(hijo)==0 solo ocurre en eliminación y se resuelve
//    con rotación simple.)
// ---------------------------------------------------------------------
template<typename K, typename V>
void balancear(std::unique_ptr<NodoBST<K,V>>& n) {
    if (!n) return;
    actualizar_altura(n);
    int fb = factor_balance(n);

    if (fb < -1) {                               // pesado a la izquierda
        if (factor_balance(n->izq) > 0)          // caso LR
            rotar_izquierda(n->izq);
        rotar_derecha(n);                        // caso LL (o 2.º paso LR)
    } else if (fb > 1) {                         // pesado a la derecha
        if (factor_balance(n->der) < 0)          // caso RL
            rotar_derecha(n->der);
        rotar_izquierda(n);                      // caso RR (o 2.º paso RL)
    }
}

// ---------------------------------------------------------------------
// TODO 4: insertar_avl — inserción BST + balancear al retornar
//   Claves duplicadas: se actualiza el valor (no se crea otro nodo).
// ---------------------------------------------------------------------
template<typename K, typename V>
void insertar_avl(std::unique_ptr<NodoBST<K,V>>& raiz,
                  K clave, V valor = V{}) {
    if (!raiz) {
        raiz = std::make_unique<NodoBST<K,V>>(std::move(clave), std::move(valor));
        return;
    }
    if (clave < raiz->clave)
        insertar_avl(raiz->izq, std::move(clave), std::move(valor));
    else if (raiz->clave < clave)
        insertar_avl(raiz->der, std::move(clave), std::move(valor));
    else {                                       // clave repetida
        raiz->valor = std::move(valor);
        return;
    }
    balancear(raiz);                             // reequilibrar al volver
}

// YA IMPLEMENTADA — no modificar
template<typename K, typename V>
NodoBST<K,V>* buscar(const std::unique_ptr<NodoBST<K,V>>& raiz,
                     const K& clave) {
    if (!raiz) return nullptr;
    if (clave == raiz->clave) return raiz.get();
    if (clave < raiz->clave) return buscar(raiz->izq, clave);
    return buscar(raiz->der, clave);
}

// =====================================================================
//  ACTIVIDAD 2A — Eliminación AVL
// =====================================================================
template<typename K, typename V>
NodoBST<K,V>* minimo(const std::unique_ptr<NodoBST<K,V>>& n) {
    return n->izq ? minimo(n->izq) : n.get();
}

template<typename K, typename V>
void eliminar_avl(std::unique_ptr<NodoBST<K,V>>& raiz,
                  const K& clave) {
    if (!raiz) return;
    if (clave < raiz->clave)
        eliminar_avl(raiz->izq, clave);
    else if (clave > raiz->clave)
        eliminar_avl(raiz->der, clave);
    else {
        if (!raiz->izq && !raiz->der) {
            // Caso 1: hoja -> se libera el nodo
            raiz = nullptr;
        } else if (!raiz->izq || !raiz->der) {
            // Caso 2: un solo hijo -> el hijo ocupa su lugar
            raiz = std::move(raiz->izq ? raiz->izq : raiz->der);
        } else {
            // Caso 3: dos hijos -> sucesor inorden (mínimo del subárbol der.)
            NodoBST<K,V>* suc = minimo(raiz->der);
            raiz->clave = suc->clave;
            raiz->valor = suc->valor;
            eliminar_avl(raiz->der, raiz->clave);  // borrar el sucesor
        }
    }
    if (raiz) balancear(raiz);   // reequilibrar
}

// =====================================================================
//  Utilidades extra (verificación y recorridos)
// =====================================================================

// Verifica las 3 propiedades: orden BST, alturas correctas y |fb| <= 1.
// Devuelve la altura real o -1 si el árbol NO es AVL válido.
template<typename K, typename V>
int verificar_avl(const std::unique_ptr<NodoBST<K,V>>& n,
                  const K* minK = nullptr, const K* maxK = nullptr) {
    if (!n) return 0;
    if (minK && !(*minK < n->clave)) return -1;
    if (maxK && !(n->clave < *maxK)) return -1;
    int hi = verificar_avl(n->izq, minK, &n->clave);
    int hd = verificar_avl(n->der, &n->clave, maxK);
    if (hi < 0 || hd < 0) return -1;
    if (hd - hi > 1 || hd - hi < -1) return -1;
    int h = 1 + std::max(hi, hd);
    return (h == n->altura) ? h : -1;
}

template<typename K, typename V, typename F>
void inorden(const std::unique_ptr<NodoBST<K,V>>& n, F visitar) {
    if (!n) return;
    inorden(n->izq, visitar);
    visitar(*n);
    inorden(n->der, visitar);
}
