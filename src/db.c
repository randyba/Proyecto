#include <stdio.h>
#include <string.h>
#include "db.h"

int db_inicializar(sqlite3 **db, const char *ruta_archivo) {
    if (sqlite3_open(ruta_archivo, db) != SQLITE_OK) {
        fprintf(stderr, "Error al abrir BD: %s\n", sqlite3_errmsg(*db));
        return 0;
    }

    // NUEVO: se agregó la columna "activo INTEGER DEFAULT 1" al final
    const char *consulta_crear_tabla =
        "CREATE TABLE IF NOT EXISTS registros ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "nombre TEXT NOT NULL, "
        "carro TEXT, "
        "placa TEXT NOT NULL, "
        "campo TEXT NOT NULL, "
        "hora_entrada DATETIME DEFAULT CURRENT_TIMESTAMP, "
        "hora_salida DATETIME, "
        "total REAL, "
        "activo INTEGER DEFAULT 1);";

    char *mensaje_error = NULL;
    if (sqlite3_exec(*db, consulta_crear_tabla, NULL, NULL, &mensaje_error) != SQLITE_OK) {
        fprintf(stderr, "Error creando tabla: %s\n", mensaje_error);
        sqlite3_free(mensaje_error);
        return 0;
    }
    return 1;
}

int db_vehiculo_activo(sqlite3 *db, const char *placa) {
    // CAMBIO: filtro ahora usa "activo = 1" en vez de "hora_salida IS NULL"
    const char *consulta =
        "SELECT COUNT(*) FROM registros WHERE placa = ? AND activo = 1;";
    sqlite3_stmt *sentencia;
    int activo = 0;

    if (sqlite3_prepare_v2(db, consulta, -1, &sentencia, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparando consulta: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    sqlite3_bind_text(sentencia, 1, placa, -1, SQLITE_STATIC);

    if (sqlite3_step(sentencia) == SQLITE_ROW) {
        activo = sqlite3_column_int(sentencia, 0) > 0;
    }
    sqlite3_finalize(sentencia);
    return activo;
}

int db_contar_disponibles(sqlite3 *db) {
    // CAMBIO: mismo filtro nuevo
    const char *consulta = "SELECT COUNT(*) FROM registros WHERE activo = 1;";
    sqlite3_stmt *sentencia;
    int ocupados = 0;

    if (sqlite3_prepare_v2(db, consulta, -1, &sentencia, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparando consulta: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    if (sqlite3_step(sentencia) == SQLITE_ROW) {
        ocupados = sqlite3_column_int(sentencia, 0);
    }
    sqlite3_finalize(sentencia);

    return CAPACIDAD_MAXIMA - ocupados;
}

int db_registrar_entrada(sqlite3 *db, const char *nombre,
                          const char *carro, const char *placa,
                          const char *campo) {
    if (db_contar_disponibles(db) <= 0) {
        fprintf(stderr, "Parqueo lleno.\n");
        return 0;
    }
    if (db_vehiculo_activo(db, placa)) {
        fprintf(stderr, "Ese vehiculo ya tiene un registro activo.\n");
        return 0;
    }

    // NOTA: no hace falta poner "activo" acá — nace en 1 solo por el
    // "DEFAULT 1" del CREATE TABLE, automático en cada INSERT.
    const char *consulta =
        "INSERT INTO registros (nombre, carro, placa, campo) "
        "VALUES (?, ?, ?, ?);";
    sqlite3_stmt *sentencia;

    if (sqlite3_prepare_v2(db, consulta, -1, &sentencia, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparando insercion: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    sqlite3_bind_text(sentencia, 1, nombre, -1, SQLITE_STATIC);
    sqlite3_bind_text(sentencia, 2, carro,  -1, SQLITE_STATIC);
    sqlite3_bind_text(sentencia, 3, placa,  -1, SQLITE_STATIC);
    sqlite3_bind_text(sentencia, 4, campo,  -1, SQLITE_STATIC);

    int resultado = sqlite3_step(sentencia);
    sqlite3_finalize(sentencia);

    if (resultado != SQLITE_DONE) {
        fprintf(stderr, "Error al insertar: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    return 1;
}

int db_registrar_salida(sqlite3 *db, const char *placa, double *monto_calculado) {
    if (!db_vehiculo_activo(db, placa)) {
        fprintf(stderr, "No hay un registro activo con esa placa.\n");
        return 0;
    }

    // CAMBIO CLAVE: se agregó "activo = 0" en el mismo UPDATE que ya cierra
    // hora_salida y calcula total — así los dos SIEMPRE cambian juntos.
    const char *consulta_actualizar =
        "UPDATE registros "
        "SET hora_salida = CURRENT_TIMESTAMP, "
        "    total = ROUND((julianday(CURRENT_TIMESTAMP) - julianday(hora_entrada)) * 24 * ?, 2), "
        "    activo = 0 "
        "WHERE placa = ? AND activo = 1;";
    sqlite3_stmt *sentencia;

    if (sqlite3_prepare_v2(db, consulta_actualizar, -1, &sentencia, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparando actualizacion: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    sqlite3_bind_double(sentencia, 1, TARIFA_POR_HORA);
    sqlite3_bind_text(sentencia, 2, placa, -1, SQLITE_STATIC);

    int resultado = sqlite3_step(sentencia);
    sqlite3_finalize(sentencia);

    if (resultado != SQLITE_DONE) {
        fprintf(stderr, "Error al registrar salida: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    const char *consulta_total =
        "SELECT total FROM registros WHERE placa = ? "
        "ORDER BY id DESC LIMIT 1;";
    if (sqlite3_prepare_v2(db, consulta_total, -1, &sentencia, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparando consulta de total: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    sqlite3_bind_text(sentencia, 1, placa, -1, SQLITE_STATIC);

    if (sqlite3_step(sentencia) == SQLITE_ROW) {
        *monto_calculado = sqlite3_column_double(sentencia, 0);
    }
    sqlite3_finalize(sentencia);

    return 1;
}

int db_cancelar_registro(sqlite3 *db, const char *placa) {
    // CAMBIO: filtro nuevo, igual que en las demás
    const char *consulta =
        "DELETE FROM registros WHERE placa = ? AND activo = 1;";
    sqlite3_stmt *sentencia;

    if (sqlite3_prepare_v2(db, consulta, -1, &sentencia, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparando eliminacion: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    sqlite3_bind_text(sentencia, 1, placa, -1, SQLITE_STATIC);
    sqlite3_step(sentencia);

    int filas_afectadas = sqlite3_changes(db);
    sqlite3_finalize(sentencia);

    return filas_afectadas > 0;
}

int db_listar_activos(sqlite3 *db, Registro lista[], int maximo) {
    // CAMBIO: se agregó la columna "activo" al SELECT, filtro pasó a "activo = 1"
    const char *consulta =
        "SELECT id, nombre, carro, placa, campo, hora_entrada, activo, "
        "ROUND((julianday('now') - julianday(hora_entrada)) * 24 * 60) AS minutos, "
        "ROUND((julianday('now') - julianday(hora_entrada)) * 24 * ?, 2) AS monto_estimado "
        "FROM registros WHERE activo = 1 LIMIT ?;";
    sqlite3_stmt *sentencia;
    int contador = 0;

    if (sqlite3_prepare_v2(db, consulta, -1, &sentencia, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparando consulta: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    sqlite3_bind_double(sentencia, 1, TARIFA_POR_HORA);
    sqlite3_bind_int(sentencia, 2, maximo);

    while (sqlite3_step(sentencia) == SQLITE_ROW && contador < maximo) {
        lista[contador].id = sqlite3_column_int(sentencia, 0);

        snprintf(lista[contador].nombre, sizeof(lista[contador].nombre),
                 "%s", (const char *)sqlite3_column_text(sentencia, 1));
        snprintf(lista[contador].carro, sizeof(lista[contador].carro),
                 "%s", (const char *)sqlite3_column_text(sentencia, 2));
        snprintf(lista[contador].placa, sizeof(lista[contador].placa),
                 "%s", (const char *)sqlite3_column_text(sentencia, 3));
        snprintf(lista[contador].campo, sizeof(lista[contador].campo),
                 "%s", (const char *)sqlite3_column_text(sentencia, 4));
        snprintf(lista[contador].hora_entrada, sizeof(lista[contador].hora_entrada),
                 "%s", (const char *)sqlite3_column_text(sentencia, 5));

        lista[contador].activo = sqlite3_column_int(sentencia, 6);
        lista[contador].minutos_transcurridos = sqlite3_column_int(sentencia, 7);
        lista[contador].monto_estimado = sqlite3_column_double(sentencia, 8);

        contador++;
    }
    sqlite3_finalize(sentencia);
    return contador;
}

// NUEVO: borra cualquier registro por su id, este activo o ya en historial.
// Este es el que va a usar el botón "Eliminar" de la interfaz.
int db_eliminar_registro(sqlite3 *db, int id) {
    const char *consulta = "DELETE FROM registros WHERE id = ?;";
    sqlite3_stmt *sentencia;

    if (sqlite3_prepare_v2(db, consulta, -1, &sentencia, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparando eliminacion: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    sqlite3_bind_int(sentencia, 1, id);
    sqlite3_step(sentencia);

    int filas_afectadas = sqlite3_changes(db);
    sqlite3_finalize(sentencia);

    return filas_afectadas > 0;
}

void db_cerrar(sqlite3 *db) {
    sqlite3_close(db);
}
