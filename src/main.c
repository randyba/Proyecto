#include <stdio.h>
#include "parqueo.h"
#include <raylib.h>

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1920; //1920
    const int screenHeight = 1000; //1080

    InitWindow(screenWidth, screenHeight, "Sistema de Parqueo");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    // espacio disponible 8(llamar desde la base de datos)
    int espacioDisponible = 3; // Este valor debería venir de la base de datos

    typedef struct{
    char owner[30];
    char placa[10];
    int espacio;
    int tiempo;
    int monto;
    int estado; 
} Auto;

Auto listaAutos[] =
{
    {"Jhon", "ABC123", 1, 180, 1500, 1},
    {"Jane", "XYZ789", 2, 120, 1000, 0},
    {"Anne", "DEF456", 3, 90, 750, 0}
};


   
    //--------------------------------------------------------------------------------------
    //coordinates for the exit button
    Rectangle boton = { 1820, 0, 100, 50 };
    Rectangle info = { 10, 300, 100, 50 };
    Rectangle btnCancelar = {980, 640, 160, 50};
    Rectangle btnGuardar = {720, 640, 160, 50};


    //variable de control para mostrar el mensaje de auto agregado
    bool autoAgregado = true;

    //variable de control para mostrar el formulario de registro de auto
    bool mostrarFormulario = false;
    
    while (!WindowShouldClose())
    {
        
        // Detectar si el mouse está sobre el botón
        Vector2 mouse = GetMousePosition();
        bool hover = CheckCollisionPointRec(mouse, boton);
        
        if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            break; // sale del loop y cierra ventana
        }

        //--------------------------------------------------------------------------------------
        // Boton para agregar un auto al parqueo (simulación)
        Vector2 center = {1820, 150};
        float radius = 30.0f;
        bool hoverCircle = CheckCollisionPointCircle(mouse, center, radius);
    
        //Funcion del boton para agregar un auto al parqueo
        if (hoverCircle && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            mostrarFormulario = true; // Mostrar el formulario de registro de auto
        }
        
        //--------------------------------------------------------------------------------------
        // Boton para cancelar el registro de auto
        bool hoverCancelar = CheckCollisionPointRec(mouse, btnCancelar);
        
        if (mostrarFormulario && hoverCancelar && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            mostrarFormulario = false;
        }
        
        //--------------------------------------------------------------------------------------
        // Boton para guardar el registro de auto
        bool hoverGuardar = CheckCollisionPointRec(mouse, btnGuardar);
        
        if (mostrarFormulario && hoverGuardar && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            mostrarFormulario = false;
            autoAgregado = true; // Mostrar el mensaje de auto agregado
            if (espacioDisponible > 0)
            {
                espacioDisponible--; // Disminuir el espacio disponible al agregar un auto
            }

        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Dibujar boton de salida
        DrawRectangleRec(boton, hover ? RED : GRAY);
        DrawText("Salir", boton.x + 25, boton.y + 15, 20, WHITE);

        //mostrar la cantidad de autos en el parqueo
        DrawText(TextFormat("Espacio disponible: %d", espacioDisponible), 10, 10, 30, BLACK);

        //--------------------------------------------------------------------------------------
        // Dibujar el círculo para agregar un auto
        DrawCircleV(center, radius, hoverCircle ? GREEN : BLUE);
        DrawCircleLines(center.x, center.y, radius, BLACK);

        //Dibujar + en el centro del círculo
        DrawText("+", center.x - 10, center.y - 20, 40, WHITE);

        //--------------------------------------------------------------------------------------
        //muestra la lista de autos en el parqueo
        if (autoAgregado)
        {
            // DrawText("Auto agregado al parqueo!", 500, 500, 20, GREEN);
            for (int i = 0; i < 3; i++){ // Mostrar el mensaje durante 1 segundo (60 frames){
                DrawRectangle(400, 200 + i*40, 500, 35, LIGHTGRAY);

                DrawText(listaAutos[i].owner, 420, 210 + i*40, 20, BLACK);

                DrawText(listaAutos[i].placa, 550, 210 + i*40, 20, BLACK);

                DrawText(TextFormat("%d", listaAutos[i].espacio),
                700, 210 + i*40, 20, BLACK);

                DrawText(TextFormat("%d min", listaAutos[i].tiempo),
                800, 210 + i*40, 20, BLACK);

                DrawText(TextFormat("₡%d", listaAutos[i].monto),
                950, 210 + i*40, 20, BLACK);
            }
        }
        
        if (mostrarFormulario)
        {

            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.35f));

            // Fondo del formulario
            DrawRectangle(550, 150, 800, 600, RAYWHITE);

            // Borde
            DrawRectangleLines(550, 150, 800, 600, BLACK);

            // Título
            DrawText("Agregar Automovil", 800, 180, 30, BLACK);

            // Labels
            DrawText("Owner:",   620, 260, 20, BLACK);
            DrawText("Placa:",   620, 330, 20, BLACK);
            DrawText("Espacio:", 620, 400, 20, BLACK);
            DrawText("Tiempo:",  620, 470, 20, BLACK);
            DrawText("Monto:",   620, 540, 20, BLACK);

            // Cajas de texto
            DrawRectangleLines(760,250,350,40,BLACK);
            DrawRectangleLines(760,320,350,40,BLACK);
            DrawRectangleLines(760,390,150,40,BLACK);
            DrawRectangleLines(760,460,150,40,BLACK);
            DrawRectangleLines(760,530,200,40,BLACK);

            // Botón Guardar
            DrawRectangleRec(btnGuardar, hoverGuardar ? GREEN : LIME);
            DrawText("Guardar",760,655,20,WHITE);

            // Botón Cancelar
            DrawRectangleRec(btnCancelar, hoverCancelar ? RED : MAROON);
            DrawText("Cancelar", 1010, 655, 20, WHITE);
        }

        EndDrawing();
        
    }

    CloseWindow();
    return 0;
}
