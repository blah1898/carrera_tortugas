#define _XOPEN_SOURCE_EXTENDED 1
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "tortuga.h"
#include "config.h"
#include "simbolos.h"

void *Tortuga_correr_hilo(void *tortuga_ptr);

int Tortuga_correr(Tortuga *tortuga, pthread_t *hilo_ptr)
{
    /* Generamos el número de pasos y el tiempo de espera */
    tortuga->pasos = rand() % (MAX_PASOS + 1 - MIN_PASOS) + MIN_PASOS;
    tortuga->espera = rand() % (MAX_ESPERA + 1 - MIN_ESPERA) + MIN_ESPERA;

    ///* Colocamos la tortuga en su posición */
    //wattron(tortuga->ventana_carrera, COLOR_PAIR(tortuga->color));
    //mvwadd_wch(tortuga->ventana_carrera, tortuga->numero + 1, tortuga->pos, &U1F422);

    //wrefresh(tortuga->ventana_carrera);
    
    /* Creamos el hilo y regresamos el valor de retorno */
    return pthread_create(hilo_ptr, NULL, Tortuga_correr_hilo, tortuga);
}

void *Tortuga_correr_hilo(void *tortuga_ptr)
{
    Tortuga *tortuga = (Tortuga *)tortuga_ptr;

    Resultado *resultado = malloc (sizeof(Resultado));

    resultado->espera = tortuga->espera;
    resultado->pasos= tortuga->pasos;

    int pos_prev = -1;

    while(! *(tortuga->carrera_terminada))
    {

        sem_wait(tortuga->semaforo_ventanas);

        if (pos_prev != -1)
        {
            mvwaddch(tortuga->ventana_carrera, tortuga->numero + 1, pos_prev + 1,' ');
        }

        wattrset(tortuga->ventana_mensajes, COLOR_PAIR(tortuga->color));
        wattrset(tortuga->ventana_carrera, COLOR_PAIR(tortuga->color));

        wprintw(tortuga->ventana_mensajes, "T%d: Avanzo %d pasos y duermo %d segundos\n", tortuga->numero, tortuga->pasos, tortuga->espera);

        if (tortuga->pos >= TAMANO_PISTA - 3)
        {
            *(tortuga->carrera_terminada) = 1;
            tortuga->pos = TAMANO_PISTA - 1;
            wattron(tortuga->ventana_carrera, A_BOLD);
            mvwadd_wch(tortuga->ventana_carrera, tortuga->numero + 1, tortuga->pos, &U1F422);
            wrefresh(tortuga->ventana_mensajes);
            wrefresh(tortuga->ventana_carrera);
            break;
        }

        mvwadd_wch(tortuga->ventana_carrera, tortuga->numero + 1, tortuga->pos, &U1F422);

        wrefresh(tortuga->ventana_mensajes);
        wrefresh(tortuga->ventana_carrera);

        sem_post(tortuga->semaforo_ventanas); 

        pos_prev = tortuga->pos;
        tortuga->pos += tortuga->pasos;

        sleep(tortuga->espera);
    }
    resultado->pasos_total = tortuga->pos;

    return resultado;
}

