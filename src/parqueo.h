#ifndef PARQUEO_H
#define PARQUEO_H

#define CAPACIDAD_MAXIMA 20

typedef struct {
    char placa[20];
    int ocupado;
} Vehiculo;

void inicializarParqueo(Vehiculo parqueo[]);
int registrarEntrada(Vehiculo parqueo[], char placa[]);
int contarEspaciosDisponibles(Vehiculo parqueo[]);

#endif
