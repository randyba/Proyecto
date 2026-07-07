#include <stdio.h>
#include "db.h"

int main(void) {
    sqlite3 *db;

    if (!db_inicializar(&db, "parqueo.db")) {
        fprintf(stderr, "No se pudo inicializar la base de datos.\n");
        return 1;
    }
    printf("Base de datos inicializada correctamente.\n");

    printf("Espacios disponibles: %d\n", db_contar_disponibles(db));

    // Prueba: registrar entrada
    if (db_registrar_entrada(db, "Jonatan", "Toyota", "ABC123", "A1")) {
        printf("Entrada registrada correctamente.\n");
    } else {
        printf("No se pudo registrar la entrada.\n");
    }

    printf("Espacios disponibles: %d\n", db_contar_disponibles(db));

    // Prueba: intentar registrar la misma placa otra vez (debe fallar)
    if (!db_registrar_entrada(db, "Otro", "Nissan", "ABC123", "A2")) {
        printf("Correcto: no dejo duplicar la placa activa.\n");
    }

    // Prueba: listar activos
    Registro lista[CAPACIDAD_MAXIMA];
    int cantidad = db_listar_activos(db, lista, CAPACIDAD_MAXIMA);
    printf("Vehiculos activos: %d\n", cantidad);
    for (int i = 0; i < cantidad; i++) {
        printf("  [%d] %s - %s - entrada: %s\n",
               lista[i].id, lista[i].nombre, lista[i].placa, lista[i].hora_entrada);
    }

    // Prueba: registrar salida
    double monto = 0.0;
    if (db_registrar_salida(db, "ABC123", &monto)) {
        printf("Salida registrada. Monto a cobrar: %.2f\n", monto);
    }

    printf("Espacios disponibles: %d\n", db_contar_disponibles(db));

    db_cerrar(db);
    return 0;
}
