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
void generar_colores();

int main(int argc, char *argv[])
{
    int max_x, max_y;
    int num_tortugas;
    int bytes_leidos;

    setlocale(LC_ALL, "");

    /* Inicialización */
    initscr();
    start_color();

    /* Generamos colores */
    generar_colores();
    cbreak(); // Recibimos sin necesitar de un enter
    keypad(stdscr, TRUE); // Recibimos teclas de números, así como funciones (F1-F13)

    /* Revisamos que la carrera quepa en pantalla */
    getmaxyx(stdscr, max_y, max_x);

    /* Revisamos que la carrera y la caja de mensajes quepan */
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
        else if (num_tortugas < MIN_TORTUGAS || num_tortugas > MAX_TORTUGAS)
        {
            printw("Ingrese un número entre %d y %d\n", MIN_TORTUGAS, MAX_TORTUGAS);
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
    WINDOW *ventana_carrera = crear_ventana_bordes(MAX_TORTUGAS + 2, // altura de MAX_TORTUGAS + 2 por los bordes
            TAMANO_PISTA + 2,  // anchura de MAX_TORTUGAS + 2 por los bordes 
            1, // Abajo de el título
            (max_x - (TAMANO_PISTA + 2)) / 2); // Centrado en X

    /* Creamos la meta */
    /* Modificamos un poco los bordes, para que quede bien dibujada la
     * meta */
    mvwadd_wch(ventana_carrera, 0, TAMANO_PISTA-1, &U252F);
    mvwadd_wch(ventana_carrera, MAX_TORTUGAS+1, TAMANO_PISTA-1, &U2537);
    /* Imprimimos la meta */
    wmove(ventana_carrera, 1, TAMANO_PISTA - 1);
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

    /* Hacemos la ventana de mensajes *scrolleable */
    scrollok(ventana_mensajes, TRUE);
    wprintw(ventana_mensajes, "Mensajes:\n" );

    refresh();
    wrefresh(ventana_mensajes);

    /* Creamos espacio para los hilos y para las tortugas */
    Tortuga tortugas[MAX_TORTUGAS];
    pthread_t hilos_tortugas[MAX_TORTUGAS];
    
    /* Creamos una variable para el estado de la carrera */
    int carrera_terminada = 0;
    sem_t semaforo_ventanas;

    /* Creamos el semáforo */
    if (sem_init (&semaforo_ventanas, 0, 1) != 0)
    {
        abortar ("ERROR: No se pudo crear el semáforo", 2);
    }

    /* Inicializamos el generador de numeros aleatorio */
    srand(time(NULL));

    /* Crear tortugas */
    for (int i = 0; i < num_tortugas; i++)
    {
        /* Inicializamos las tortugas y las corremos */
        tortugas[i].color = i+1;
        tortugas[i].numero = i;
        tortugas[i].pos = 1;
        tortugas[i].carrera_terminada = &carrera_terminada;
        tortugas[i].ventana_carrera = ventana_carrera;
        tortugas[i].ventana_mensajes = ventana_mensajes;
        tortugas[i].semaforo_ventanas = &semaforo_ventanas;
        /* Generamos el número de pasos y el tiempo de espera */
        tortugas[i].pasos = rand() % (MAX_PASOS + 1 - MIN_PASOS) + MIN_PASOS;
        tortugas[i].espera = rand() % (MAX_ESPERA + 1 - MIN_ESPERA) + MIN_ESPERA;

        if (pthread_create(&hilos_tortugas[i], NULL, Tortuga_correr_hilo, &tortugas[i]) != 0)
        {
            char mensaje[50];
            sprintf (mensaje, "ERROR: No se pudo crear tortuga %d: %d", i, errno);
            getch();
            endwin();
            return 3;
        }
    }

    /* Hacemos espacio para los resultados */
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
        wprintw(ventana_mensajes, "Tortuga %d se movía %d, esperaba %d, y en total se movió %d\n", i, resultados[i]->pasos, resultados[i]->espera, resultados[i]->pasos_total);
        /* Eliminamos el resultado ya no necesitado */
        free(resultados[i]);
    }

    wrefresh(ventana_mensajes);

    getch();

    delwin(ventana_mensajes);
    delwin(ventana_carrera);
    delwin(ventana_mensajes_borde);

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

    /* Creamos la ventana */
    ventana_local = newwin(altura, anchura, inicio_y, inicio_x);
    /* Los bordes los definimos con estructuras definidas en simbolos.h */
    wborder_set(ventana_local, &U2503, &U2503, &U2501, &U2501, &U250F, &U2513, &U2517, &U251B);
    wrefresh(ventana_local);

    return ventana_local;
}

void generar_colores()
{
    for (int i = 0; i < MAX_TORTUGAS; i++) {
        /* Esto genera colores que no son tan oscuros como para que no
         * se vean */
        div_t division = div(i+10, 8);
        init_pair(i+1, division.quot * 20 + division.rem, 0);
    }
}
