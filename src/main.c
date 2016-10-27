#define _XOPEN_SOURCE_EXTENDED 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <locale.h>
#include <ncurses.h>
#include "config.h"
#include "simbolos.h"

WINDOW *crear_ventana_bordes(int altura, int anchura, int inicio_y, int inicio_x);
void abortar(const char *mensaje, int cod_error);

int main(int argc, char *argv[])
{
    int max_x, max_y;
    int centro_x, centro_y; /* Para hacer centrados */
    int num_tortugas;
    int bytes_leidos;

    setlocale(LC_ALL, "");

    /* Inicialización */
    initscr();
    cbreak(); // Recibimos sin necesitar de un enter
    keypad(stdscr, TRUE); // Recibimos teclas de números, así como funciones (F1-F13)

    /* Revisamos que la carrera quepa en pantalla */
    getmaxyx(stdscr, max_y, max_x);

    if (max_x < COLUMNAS + 2 || max_y < (FILAS + 6))
    {
        /* Si no cabe la carrera, terminamos */
        char mensaje[50];
        sprintf(mensaje, "Esta aplicación requiere de una terminal de %dx%d.", COLUMNAS + 2, FILAS+6);
        abortar(mensaje, 1);
    }

    /* Pedimos el número de tortugas */
    int valido;
    do {
        printw("Ingrese el número de tortugas: ");
        bytes_leidos = scanw("%d", &num_tortugas);
        clear();
        if (bytes_leidos < 1)
        {
            printw("Entrada inválida.\n");
            valido = FALSE;
        }
        else if (num_tortugas < 2 || num_tortugas > 10)
        {
            printw("Ingrese un número entre 2 y 10\n");
            valido = FALSE;
        }
        else
        {
            valido = TRUE;
        }
    } while (!valido);

    noecho();
    refresh();


    /* Imprimimos el título */
    const char *titulo = "CARRERA DE TORTUGAS";
    attron(A_BOLD);
    mvaddstr(0, (max_x - strlen(titulo)) / 2, titulo);
    attrset(A_NORMAL);

    /* Creamos la ventana de la carrera */
    WINDOW *ventana_carrera = crear_ventana_bordes(FILAS + 2, // Numero filas + bordes
            COLUMNAS + 2, // Numero columnas + bordes
            1, // Iniciamos uno abajo
            (max_x - (COLUMNAS + 2)) / 2);

    /* Creamos la meta */
    /* Modificamos un poco los bordes */
    mvwadd_wch(ventana_carrera, 0, COLUMNAS-3, &U252F);
    mvwadd_wch(ventana_carrera, FILAS+1, COLUMNAS-3, &U2537);
    /* Imprimimos la meta */
    wmove(ventana_carrera, 1, COLUMNAS - 3);
    wvline_set(ventana_carrera, &U254E, FILAS);

    wrefresh(ventana_carrera);

    /* Creamos la ventana de mensajes */
    WINDOW *ventana_mensajes_borde = crear_ventana_bordes(max_y - FILAS - 3,
            max_x,
            FILAS + 3,
            0);

    WINDOW *ventana_mensajes = newwin(max_y - FILAS - 5,
            max_x-2,
            FILAS + 4,
            1);

    scrollok(ventana_mensajes, TRUE);
    // Movemos el cursor a la salida
    wmove (ventana_mensajes, 0, 0);
    
    for(int i = 0; i < 100; i++) {
        wprintw(ventana_mensajes, "%d\n", i);
    }

    refresh();
    wrefresh(ventana_mensajes);

    while (TRUE)
    {
        if(getch() == 'x')
        {
            break;
        };
    }

    delwin(ventana_mensajes);
    delwin(ventana_carrera);

    endwin();
}

void abortar(const char *mensaje, int cod_error)
{
    if (mensaje)
    {
        printw(mensaje);
    }
    
    printw("\n");
    printw("Pulse cualquier tecla para continuar...");

    getch();
    endwin();
    exit(cod_error);
}

WINDOW *crear_ventana_bordes(int altura, int anchura, int inicio_y, int inicio_x)
{
    WINDOW *ventana_local;

    ventana_local = newwin(altura, anchura, inicio_y, inicio_x);
    wborder_set(ventana_local, &U2503, &U2503, &U2501, &U2501, &U250F, &U2513, &U2517, &U251B);
    wrefresh(ventana_local);


    return ventana_local;
}
