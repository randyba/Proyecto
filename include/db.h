#ifndef DB_H
#define DB_H

#include <sqlite3.h>

#define CAPACIDAD_MAXIMA 20
#define TARIFA_POR_HORA  500.0   // colones por hora — ajustar según enunciado

typedef struct {
    int    id;
    char   nombre[50];
    char   carro[30];
    char   placa[20];
    char   campo[10];
    char   hora_entrada[20];
} Registro;

// Inicializa la conexion y crea la tabla si no existe
int  db_inicializar(sqlite3 **db, const char *ruta_archivo);

// Retorna 1 si esa placa ya tiene un registro activo (sin salida), 0 si no
int  db_vehiculo_activo(sqlite3 *db, const char *placa);

// Registra entrada. Retorna 1 si tuvo exito, 0 si fallo (lleno o ya activo)
int  db_registrar_entrada(sqlite3 *db, const char *nombre,
                           const char *carro, const char *placa,
                           const char *campo);

// Registra salida y calcula el cobro. Escribe el monto en *monto_calculado
// Retorna 1 si tuvo exito, 0 si no habia registro activo con esa placa
int  db_registrar_salida(sqlite3 *db, const char *placa, double *monto_calculado);

// Elimina un registro activo sin cobrar (boton "Cancelar" del mockup)
int  db_cancelar_registro(sqlite3 *db, const char *placa);

// Espacios libres = CAPACIDAD_MAXIMA - vehiculos activos
int  db_contar_disponibles(sqlite3 *db);

// Llena 'lista' con hasta 'maximo' registros activos. Retorna cuantos lleno
int  db_listar_activos(sqlite3 *db, Registro lista[], int maximo);

// Cierra la conexion
void db_cerrar(sqlite3 *db);

#endif // DB_H
