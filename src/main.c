#define _XOPEN_SOURCE_EXTENDED 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <locale.h>
#include <ncurses.h>
#include <errno.h>
#include "tortuga.h"
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
    start_color();

    /* Generamos colores */
    init_pair(1, 1, 0);
    init_pair(2, 2, 0);
    init_pair(3, 3, 0);
    init_pair(4, 4, 0);
    init_pair(5, 5, 0);
    init_pair(6, 6, 0);
    init_pair(7, 31, 0);
    init_pair(8, 34, 0);
    init_pair(9, 35, 0);
    init_pair(10, 38, 0);

    cbreak(); // Recibimos sin necesitar de un enter
    keypad(stdscr, TRUE); // Recibimos teclas de números, así como funciones (F1-F13)

    /* Revisamos que la carrera quepa en pantalla */
    getmaxyx(stdscr, max_y, max_x);

    if (max_x < TAMANO_PISTA + 2 || max_y < (MAX_TORTUGAS + TAMANO_MIN_MENSAJES + 4))
    {
        /* Si no cabe la carrera, terminamos */
        char mensaje[50];
        sprintf(mensaje, "Esta aplicación requiere de una terminal de %dx%d.", TAMANO_PISTA + 2, (MAX_TORTUGAS + TAMANO_MIN_MENSAJES + 4));
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
    attron(COLOR_PAIR(1));
    mvaddstr(0, (max_x - strlen(titulo)) / 2, titulo);
    attrset(A_NORMAL);

    /* Creamos la ventana de la carrera */
    WINDOW *ventana_carrera = crear_ventana_bordes(MAX_TORTUGAS + 2, 
            TAMANO_PISTA + 2, 
            1, // Iniciamos uno abajo
            (max_x - (TAMANO_PISTA + 2)) / 2);

    /* Creamos la meta */
    /* Modificamos un poco los bordes */
    mvwadd_wch(ventana_carrera, 0, TAMANO_PISTA-2, &U252F);
    mvwadd_wch(ventana_carrera, MAX_TORTUGAS+1, TAMANO_PISTA-2, &U2537);
    /* Imprimimos la meta */
    wmove(ventana_carrera, 1, TAMANO_PISTA - 2);
    wvline_set(ventana_carrera, &U254E, MAX_TORTUGAS);

    wrefresh(ventana_carrera);

    /* Creamos la ventana de mensajes */
    WINDOW *ventana_mensajes_borde = crear_ventana_bordes(max_y - MAX_TORTUGAS - 3,
            max_x,
            MAX_TORTUGAS + 3,
            0);

    WINDOW *ventana_mensajes = newwin(max_y - MAX_TORTUGAS - 5,
            max_x-2,
            MAX_TORTUGAS + 4,
            1);

    scrollok(ventana_mensajes, TRUE);
    // Movemos el cursor a la salida
    wprintw(ventana_mensajes, "Mensajes:\n" );

    refresh();
    wrefresh(ventana_mensajes);

    Tortuga tortugas[10];
    pthread_t hilos_tortugas[10];
    
    // Aqui se almacena si la carrera ya terminó
    int carrera_terminada = 0;
    sem_t semaforo_ventanas;

    if (sem_init (&semaforo_ventanas, 0, 1) != 0)
    {
        wprintw (ventana_mensajes, "ERROR: No se pudo crear el semáforo");
        endwin();
        return 2;

    }

    srand( time(NULL));

    /* Crear tortugas */
    for (int i = 0; i < num_tortugas; i++)
    {
        tortugas[i].color = i+1;
        tortugas[i].numero = i;
        tortugas[i].pos = 1;
        tortugas[i].carrera_terminada = &carrera_terminada;
        tortugas[i].ventana_carrera = ventana_carrera;
        tortugas[i].ventana_mensajes = ventana_mensajes;
        tortugas[i].semaforo_ventanas = &semaforo_ventanas;

        if (Tortuga_correr (&tortugas[i], &hilos_tortugas[i]) != 0)
        {
            wprintw (ventana_mensajes, "ERROR: No se pudo crear tortuga %d: %d", i, errno);
            getch();
            endwin();
            return 3;
        }
    }

    Resultado *resultados[10];

    /* Esperar tortugas*/
    for (int i = 0; i < num_tortugas; i++)
    {
        pthread_join(hilos_tortugas[i], (void **)&resultados[i]);
    }
    /* Imprimir resultados */

    for (int i = 0; i < num_tortugas; i++)
    {
        wattrset(ventana_mensajes, A_NORMAL);
        wprintw(ventana_mensajes, "Tortuga %d se movía %d, esperaba %d, y en total se movió %d\n", i, resultados[i]->pasos_periodo, resultados[i]->tiempo_descanso, resultados[i]->pasos_dados);
    }

    wrefresh(ventana_mensajes);

    getch();

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
