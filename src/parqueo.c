#include <string.h>
#include "parqueo.h"

void inicializarParqueo(Vehiculo parqueo[]) {

    for (int i = 0; i < CAPACIDAD_MAXIMA; i++) {
        parqueo[i].ocupado = 0;
        parqueo[i].placa[0] = '\0';
    }
}

int buscarVehiculoPorPlaca(Vehiculo parqueo[], char placa[]) {

    for (int i = 0; i < CAPACIDAD_MAXIMA; i++) {
        if (parqueo[i].ocupado == 1 && strcmp(parqueo[i].placa, placa) == 0) {
            return i;
        }
    }

    return -1;
}

int registrarEntrada(Vehiculo parqueo[], char placa[]) {

    if (buscarVehiculoPorPlaca(parqueo, placa) != -1) {
        return -1;
    }

    for (int i = 0; i < CAPACIDAD_MAXIMA; i++) {

        if (parqueo[i].ocupado == 0) {

            strcpy(parqueo[i].placa, placa);
            parqueo[i].ocupado = 1;

            return 1;
        }
    }

    return 0;
}

int registrarSalida(Vehiculo parqueo[], char placa[]) {

    int posicion = buscarVehiculoPorPlaca(parqueo, placa);

    if (posicion == -1) {
        return 0;
    }

    parqueo[posicion].ocupado = 0;
    parqueo[posicion].placa[0] = '\0';

    return 1;
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
