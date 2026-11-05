#include <iostream>
#include <thread>
#include <mutex>
#include <random>
#include <chrono>
#include <string>
using namespace std;

struct Nodo {
    int dato;
    Nodo* sig;
    Nodo(int d) : dato(d), sig(nullptr) {}
};

class ListaSegura {
private:
    Nodo* cabeza;
    mutex candado;

public:
    ListaSegura() : cabeza(nullptr) {}

    ~ListaSegura() {
        Nodo* act = cabeza;
        while (act != nullptr) {
            Nodo* tmp = act->sig;
            delete act;
            act = tmp;
        }
    }

    void insertar(int valor) {
        lock_guard<mutex> lk(candado);
        Nodo* nuevo = new Nodo(valor);
        nuevo->sig = cabeza;
        cabeza = nuevo;
    }

    bool eliminar(int valor) {
        lock_guard<mutex> lk(candado);
        Nodo* act = cabeza;
        Nodo* ant = nullptr;
        while (act != nullptr && act->dato != valor) {
            ant = act;
            act = act->sig;
        }
        if (act == nullptr) return false;
        if (ant != nullptr) ant->sig = act->sig;
        else cabeza = act->sig;
        delete act;
        return true;
    }

    bool buscar(int valor) {
        lock_guard<mutex> lk(candado);
        for (Nodo* p = cabeza; p != nullptr; p = p->sig)
            if (p->dato == valor) return true;
        return false;
    }

    bool modificar(int viejo, int nuevoValor) {
        lock_guard<mutex> lk(candado);
        for (Nodo* p = cabeza; p != nullptr; p = p->sig) {
            if (p->dato == viejo) {
                p->dato = nuevoValor;
                return true;
            }
        }
        return false;
    }

    void mostrar() {
        lock_guard<mutex> lk(candado);
        for (Nodo* p = cabeza; p != nullptr; p = p->sig)
            cout << p->dato << " ";
        cout << endl;
    }
};

mutex consola;

void imprimir(const string& msg) {
    lock_guard<mutex> lk(consola);
    cout << msg << endl;
}

int aleatorio(mt19937& gen, int a, int b) {
    uniform_int_distribution<int> dist(a, b);
    return dist(gen);
}

void pausa(mt19937& gen) {
    this_thread::sleep_for(chrono::milliseconds(aleatorio(gen, 10, 60)));
}

void tareaEliminar(ListaSegura& lista) {
    random_device rd; mt19937 gen(rd());
    for (int i = 0; i < 10; i++) {
        int n = aleatorio(gen, 1, 30);
        if (lista.eliminar(n)) imprimir("Hilo 1 -> Eliminando: " + to_string(n));
        else                   imprimir("Hilo 1 -> No se eliminó " + to_string(n));
        pausa(gen);
    }
}

void tareaInsertar(ListaSegura& lista) {
    random_device rd; mt19937 gen(rd());
    for (int i = 0; i < 10; i++) {
        int n = aleatorio(gen, 1, 30);
        lista.insertar(n);
        imprimir("Hilo 2 -> Insertando: " + to_string(n));
        pausa(gen);
    }
}

void tareaBuscar(ListaSegura& lista) {
    random_device rd; mt19937 gen(rd());
    for (int i = 0; i < 10; i++) {
        int n = aleatorio(gen, 1, 30);
        if (lista.buscar(n)) imprimir("Hilo 3 -> Buscado: " + to_string(n));
        else                 imprimir("Hilo 3 -> No encontrado: " + to_string(n));
        pausa(gen);
    }
}

void tareaModificar(ListaSegura& lista) {
    random_device rd; mt19937 gen(rd());
    for (int i = 0; i < 10; i++) {
        int n = aleatorio(gen, 1, 30);
        int m = n + aleatorio(gen, 1, 9);
        if (lista.modificar(n, m))
            imprimir("Hilo 4 -> Modificando " + to_string(n) + " a " + to_string(m));
        else
            imprimir("Hilo 4 -> No se encontró " + to_string(n));
        pausa(gen);
    }
}

int main() {
    ListaSegura lista;

    random_device rd; mt19937 gen(rd());
    for (int i = 0; i < 10; i++) lista.insertar(aleatorio(gen, 1, 30));

    cout << "Lista inicial: ";
    lista.mostrar();
    cout << "---------------------------" << endl;

    thread h1(tareaEliminar, ref(lista));
    thread h2(tareaInsertar, ref(lista));
    thread h3(tareaBuscar,   ref(lista));
    thread h4(tareaModificar, ref(lista));

    h1.join(); h2.join(); h3.join(); h4.join();

    cout << "---------------------------" << endl;
    cout << "Lista final:   ";
    lista.mostrar();
    return 0;
}
