#ifndef DB_H
#define DB_H

#include <sqlite3.h>

int db_inicializar(sqlite3 **db);
int db_registrar_entrada(sqlite3 *db, const char *placa, const char *nombre);
int db_registrar_salida(sqlite3 *db, const char *placa);

#endif
