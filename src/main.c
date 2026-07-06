#include <stdio.h>
#include <string.h>
#include "db.h"
#include <raylib.h>

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1920; //1920
    const int screenHeight = 1000; //1080

    InitWindow(screenWidth, screenHeight, "Sistema de Parqueo");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    sqlite3 *db;
    if (!db_inicializar(&db, "parqueo.db")) {
        fprintf(stderr, "No se pudo inicializar la base de datos.\n");
        CloseWindow();
        return 1;
    }

    int espacioDisponible = db_contar_disponibles(db);

    Registro listaAutos[CAPACIDAD_MAXIMA];
    int cantidadAutos = db_listar_activos(db, listaAutos, CAPACIDAD_MAXIMA);

    float temporizadorRefresco = 0.0f;

    //--------------------------------------------------------------------------------------
    Rectangle boton = { 1820, 0, 100, 50 };
    Rectangle info = { 10, 300, 100, 50 };
    Rectangle btnCancelar = {980, 640, 160, 50};
    Rectangle btnGuardar = {720, 640, 160, 50};

    Rectangle cajaOwner   = {760, 250, 350, 40};
    Rectangle cajaPlaca   = {760, 320, 350, 40};
    Rectangle cajaCarro   = {760, 390, 350, 40};
    Rectangle cajaEspacio = {760, 460, 150, 40};
    Rectangle cajaMonto   = {760, 530, 200, 40};


    char inputOwner[30]   = "";
    char inputPlaca[10]   = "";
    char inputCarro[50]   = "";
    char inputEspacio[5]  = "";

    int campoActivo = -1;
    
    // ---- NUEVO: mensaje de error del formulario (placa duplicada, lleno, campos vacíos) ----
    char mensajeError[100] = "";
    float tiempoMensajeError = 0.0f; // cuánto tiempo más se muestra el mensaje
    
    bool autoAgregado = true;
    bool mostrarFormulario = false;
    
    
    while (!WindowShouldClose())
    {
        int totalMonto = 0;
        temporizadorRefresco += GetFrameTime();
        if (temporizadorRefresco >= 1.0f) {
            temporizadorRefresco = 0.0f;
            cantidadAutos = db_listar_activos(db, listaAutos, CAPACIDAD_MAXIMA);
            espacioDisponible = db_contar_disponibles(db);
        }

        // NUEVO: contador regresivo para que el mensaje de error desaparezca solo
        if (tiempoMensajeError > 0.0f) {
            tiempoMensajeError -= GetFrameTime();
            if (tiempoMensajeError <= 0.0f) mensajeError[0] = '\0';
        }

        Vector2 mouse = GetMousePosition();
        bool hover = CheckCollisionPointRec(mouse, boton);
        
        if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            break;
        }

        //--------------------------------------------------------------------------------------
        Vector2 center = {1820, 150};
        float radius = 30.0f;
        bool hoverCircle = CheckCollisionPointCircle(mouse, center, radius);
    
        if (hoverCircle && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            mostrarFormulario = true;
        }
        //--------------------------------------------------------------------------------------
        //--------------------------------------------------------------------------------------
        bool hoverCancelar = CheckCollisionPointRec(mouse, btnCancelar);
        
        if (mostrarFormulario && hoverCancelar && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            mostrarFormulario = false;
            inputOwner[0] = '\0';
            inputPlaca[0] = '\0';
            inputCarro[0] = '\0';
            inputEspacio[0] = '\0';
            campoActivo = -1;
            mensajeError[0] = '\0';
        }
        
        //--------------------------------------------------------------------------------------
        bool hoverGuardar = CheckCollisionPointRec(mouse, btnGuardar);
        
        if (mostrarFormulario && hoverGuardar && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            // NUEVO: validar que al menos placa y campo no vengan vacíos antes de llamar a la BD
            if (strlen(inputPlaca) == 0 || strlen(inputEspacio) == 0)
            {
                snprintf(mensajeError, sizeof(mensajeError), "Placa y Espacio son obligatorios.");
                tiempoMensajeError = 3.0f;
            }
            else
            {
                // ---- NUEVO: la llamada real a la base de datos ----
                if (db_registrar_entrada(db, inputOwner, inputCarro, inputPlaca, inputEspacio))
                {
                    // éxito: cerramos el formulario y refrescamos YA, sin esperar el timer de 1 seg
                    mostrarFormulario = false;
                    cantidadAutos = db_listar_activos(db, listaAutos, CAPACIDAD_MAXIMA);
                    espacioDisponible = db_contar_disponibles(db);

                    inputOwner[0] = '\0';
                    inputPlaca[0] = '\0';
                    inputCarro[0] = '\0';
                    inputEspacio[0] = '\0';
                    campoActivo = -1;
                    mensajeError[0] = '\0';
                }
                else
                {
                    // fracaso: NO cerramos el formulario, así el usuario ve el error y corrige
                    snprintf(mensajeError, sizeof(mensajeError),
                        "No se pudo registrar (parqueo lleno o placa ya activa).");
                    tiempoMensajeError = 3.0f;
                }
            }
        }
        
        if (mostrarFormulario)
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                if (CheckCollisionPointRec(mouse, cajaOwner))        campoActivo = 0;
                else if (CheckCollisionPointRec(mouse, cajaPlaca))   campoActivo = 1;
                else if (CheckCollisionPointRec(mouse, cajaCarro))   campoActivo = 2;
                else if (CheckCollisionPointRec(mouse, cajaEspacio)) campoActivo = 3;
            }

            char *bufferActivo = NULL;
            int maxLen = 0;
            switch (campoActivo)
            {
                case 0: bufferActivo = inputOwner;   maxLen = sizeof(inputOwner);   break;
                case 1: bufferActivo = inputPlaca;   maxLen = sizeof(inputPlaca);   break;
                case 2: bufferActivo = inputCarro;   maxLen = sizeof(inputCarro);   break;
                case 3: bufferActivo = inputEspacio; maxLen = sizeof(inputEspacio); break;
                default: bufferActivo = NULL; break;
            }

            if (bufferActivo != NULL)
            {
                int tecla = GetCharPressed();
                while (tecla > 0)
                {
                    int len = strlen(bufferActivo);
                    if (len < maxLen - 1 && tecla >= 32 && tecla <= 125)
                    {
                        bufferActivo[len] = (char)tecla;
                        bufferActivo[len + 1] = '\0';
                    }
                    tecla = GetCharPressed();
                }

                if (IsKeyPressed(KEY_BACKSPACE))
                {
                    int len = strlen(bufferActivo);
                    if (len > 0) bufferActivo[len - 1] = '\0';
                }
            }
        }
        

        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        DrawRectangleRec(boton, hover ? RED : GRAY);
        DrawText("Salir", boton.x + 25, boton.y + 15, 20, WHITE);

        DrawText(TextFormat("Espacio disponible: %d", espacioDisponible), 10, 10, 30, BLACK);

        DrawCircleV(center, radius, hoverCircle ? GREEN : BLUE);
        DrawCircleLines(center.x, center.y, radius, BLACK);
        DrawText("+", center.x - 10, center.y - 20, 40, WHITE);      
        
        
        
        
        if (autoAgregado){
            // Encabezado
            DrawRectangle(400, 160, 600, 35, GRAY);
            
            DrawText("Owner",   420, 170, 20, BLACK);
            DrawText("Carro",   550, 170, 20, BLACK);
            DrawText("Placa",   650, 170, 20, BLACK);
            DrawText("Campo",   750, 170, 20, BLACK);
            DrawText("Tiempo",  820, 170, 20, BLACK);
    DrawText("Monto",   920, 170, 20, BLACK);

    // Datos
    for (int i = 0; i < cantidadAutos; i++)
    {
        if (listaAutos[i].activo == 1)
        {

            totalMonto += (int)listaAutos[i].monto_estimado;
            DrawRectangle(400, 220 + i*40, 600, 50, LIGHTGRAY);
            
            DrawText(listaAutos[i].nombre, 420, 240 + i*40, 20, BLACK);
            DrawText(listaAutos[i].carro, 550, 240 + i*40, 20, BLACK);
            DrawText(listaAutos[i].placa, 650, 240 + i*40, 20, BLACK);
            DrawText(listaAutos[i].campo, 750, 240 + i*40, 20, BLACK);
            
            DrawText(TextFormat("%d min", listaAutos[i].minutos_transcurridos),
            820, 240 + i*40, 20, BLACK);
            
            DrawText(TextFormat("$%.0f", listaAutos[i].monto_estimado),
            920, 240 + i*40, 20, BLACK);
            
            // Botón "Caducar"
            Rectangle btnCaducar = {1025, 230 + i*40, 30, 30};
            
            
            
            bool hoverCaducar = CheckCollisionPointRec(mouse, btnCaducar);
            if (autoAgregado && hoverCaducar && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                // // NUEVO: llamar a la función de la base de datos para caducar el registro
                if (db_cancelar_registro(db, listaAutos[0].placa)) {
                    // éxito: refrescamos YA, sin esperar el timer de 1 seg
                    cantidadAutos = db_listar_activos(db, listaAutos, CAPACIDAD_MAXIMA);
                    espacioDisponible = db_contar_disponibles(db);
                } else {
                    // fracaso: mostrar mensaje de error (aunque no debería pasar)
                    snprintf(mensajeError, sizeof(mensajeError),
                        "No se pudo caducar el registro.");
                    tiempoMensajeError = 3.0f;
                }
                listaAutos[0].activo = 0; // marcar como inactivo para que desaparezca de la lista
            }

            DrawRectangleRec(btnCaducar, hoverCaducar ? RED : MAROON);
            DrawText("X", btnCaducar.x + 6, btnCaducar.y + 6, 20, WHITE);
        }
        
    }
}
        
if (mostrarFormulario)
{
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.35f));
    DrawRectangle(550, 150, 800, 600, RAYWHITE);
    DrawRectangleLines(550, 150, 800, 600, BLACK);
    DrawText("Agregar Automovil", 800, 180, 30, BLACK);
    
    DrawText("Owner:",  620, 260, 20, BLACK);
    DrawText("Placa:",  620, 330, 20, BLACK);
    DrawText("Carro:",  620, 400, 20, BLACK);
    DrawText("Espacio:",620, 470, 20, BLACK);
    
    DrawRectangleLines(cajaOwner.x, cajaOwner.y, cajaOwner.width, cajaOwner.height,
        campoActivo == 0 ? BLUE : BLACK);
        DrawRectangleLines(cajaPlaca.x, cajaPlaca.y, cajaPlaca.width, cajaPlaca.height,
            campoActivo == 1 ? BLUE : BLACK);
            DrawRectangleLines(cajaCarro.x, cajaCarro.y, cajaCarro.width, cajaCarro.height,
                campoActivo == 2 ? BLUE : BLACK);
                DrawRectangleLines(cajaEspacio.x, cajaEspacio.y, cajaEspacio.width, cajaEspacio.height,
                    campoActivo == 3 ? BLUE : BLACK);
                    
                    DrawText(inputOwner,   cajaOwner.x + 8,   cajaOwner.y + 10,   20, BLACK);
                    DrawText(inputPlaca,   cajaPlaca.x + 8,   cajaPlaca.y + 10,   20, BLACK);
                    DrawText(inputCarro,   cajaCarro.x + 8,   cajaCarro.y + 10,   20, BLACK);
                    DrawText(inputEspacio, cajaEspacio.x + 8, cajaEspacio.y + 10, 20, BLACK);
                    
                    // NUEVO: mostrar el mensaje de error, si hay uno activo
                    if (strlen(mensajeError) > 0) {
                        DrawText(mensajeError, 620, 600, 18, RED);
                    }
                    
            DrawRectangleRec(btnGuardar, hoverGuardar ? GREEN : LIME);
            DrawText("Guardar",760,655,20,WHITE);

            DrawRectangleRec(btnCancelar, hoverCancelar ? RED : MAROON);
            DrawText("Cancelar", 1010, 655, 20, WHITE);

        }
        
        DrawText(TextFormat("TOTAL: $ %d", totalMonto), 1400, 900, 60, BLACK);
        DrawText(TextFormat("SISTEMA DE PARQUEO"), 700, 60, 50, BLACK);

        EndDrawing();
        
    }

    db_cerrar(db);
    CloseWindow();
    return 0;
}