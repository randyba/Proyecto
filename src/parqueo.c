#include <string.h>
#include "parqueo.h"

void inicializarParqueo(Vehiculo parqueo[]) {

    for (int i = 0; i < CAPACIDAD_MAXIMA; i++) {
        parqueo[i].ocupado = 0;
    }
}

int registrarEntrada(Vehiculo parqueo[], char placa[]) {

    for (int i = 0; i < CAPACIDAD_MAXIMA; i++) {

        if (parqueo[i].ocupado == 0) {

            strcpy(parqueo[i].placa, placa);
            parqueo[i].ocupado = 1;

            return 1;
        }
    }

    return 0;
}

int contarEspaciosDisponibles(Vehiculo parqueo[]) {

    int disponibles = 0;

    for (int i = 0; i < CAPACIDAD_MAXIMA; i++) {

        if (parqueo[i].ocupado == 0) {
            disponibles++;
        }
    }

    return disponibles;
}
