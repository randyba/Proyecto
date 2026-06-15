#include <stdio.h>
#include "parqueo.h"

int main() {

    Vehiculo parqueo[CAPACIDAD_MAXIMA];
    char placa[20];

    inicializarParqueo(parqueo);

    printf("Ingrese la placa del vehiculo: ");
    scanf("%s", placa);

    if (registrarEntrada(parqueo, placa)) {
        printf("Vehiculo registrado correctamente.\n");
    } else {
        printf("No fue posible registrar el vehiculo.\n");
    }

    printf("Espacios disponibles: %d\n",
           contarEspaciosDisponibles(parqueo));

    return 0;
}
