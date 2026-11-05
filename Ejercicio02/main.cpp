
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <thread>
#include <cmath>
#include <iomanip>

using namespace std;

struct Registro {
    string origen;
    string destino;
    double x1, y1, x2, y2;
    double euclidea;
    double manhattan;
};

void calcularEuclidea(vector<Registro>& datos) {
    for (auto& r : datos) {
        double dx = r.x2 - r.x1;
        double dy = r.y2 - r.y1;

        r.euclidea = sqrt(dx * dx + dy * dy);
    }
}

void calcularManhattan(vector<Registro>& datos) {
    for (auto& r : datos) {
        r.manhattan = abs(r.x2 - r.x1)
                    + abs(r.y2 - r.y1);
    }
}

bool cargarCSV(const string& archivo, vector<Registro>& datos) {
    ifstream entrada(archivo);

    if (!entrada.is_open()) {
        cerr << "Error al abrir el archivo CSV." << endl;
        return false;
    }

    string linea;
    getline(entrada, linea); // Omitir encabezado

    while (getline(entrada, linea)) {
        if (linea.empty()) {
            continue;
        }

        stringstream ss(linea);
        string campo;
        Registro r{};

        try {
            getline(ss, r.origen, ',');
            getline(ss, r.destino, ',');

            getline(ss, campo, ',');
            r.x1 = stod(campo);

            getline(ss, campo, ',');
            r.y1 = stod(campo);

            getline(ss, campo, ',');
            r.x2 = stod(campo);

            getline(ss, campo, ',');
            r.y2 = stod(campo);

            datos.push_back(r);
        }
        catch (const exception& e) {
            cerr << "Registro invalido: " << linea << endl;
        }
    }

    return true;
}

int main() {
    vector<Registro> datos;

    if (!cargarCSV("distancias_manhattan_euclides.csv", datos)) {
        return 1;
    }

    if (datos.empty()) {
        cout << "No hay registros para procesar." << endl;
        return 1;
    }

    thread hilo1(calcularEuclidea, ref(datos));
    thread hilo2(calcularManhattan, ref(datos));

    hilo1.join();
    hilo2.join();

    cout << fixed << setprecision(4);

    cout << "PRIMEROS 5 REGISTROS" << endl;
    cout << "----------------------------------------" << endl;

    size_t limite = min(size_t(5), datos.size());

    for (size_t i = 0; i < limite; i++) {
        const Registro& r = datos[i];

        cout << "\nRegistro " << i + 1 << endl;
        cout << "Origen: " << r.origen << endl;
        cout << "Destino: " << r.destino << endl;
        cout << "Distancia Euclidea: " << r.euclidea << endl;
        cout << "Distancia Manhattan: " << r.manhattan << endl;
    }

    cout << "\nTotal de registros procesados: "
         << datos.size() << endl;

    return 0;
}
