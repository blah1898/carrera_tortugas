#define _XOPEN_SOURCE_EXTENDED 1
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "tortuga.h"
#include "config.h"
#include "simbolos.h"

void *Tortuga_correr_hilo(void *tortuga_ptr);

int Tortuga_correr(Tortuga *tortuga, pthread_t *hilo_ptr)
{
    /* Generamos el número de pasos y el tiempo de espera */
    tortuga->pasos = rand() % 10 + 1;
    tortuga->espera = rand() % 5 + 1;

    /* Colocamos la tortuga en su posición */
    wattron(tortuga->ventana_carrera, COLOR_PAIR(tortuga->color));
    mvwadd_wch(tortuga->ventana_carrera, tortuga->numero + 1, tortuga->pos, &U1F422);

    wrefresh(tortuga->ventana_carrera);
    
    /* Creamos el hilo y regresamos el valor de retorno */
    return pthread_create(hilo_ptr, NULL, Tortuga_correr_hilo, tortuga);
}

void *Tortuga_correr_hilo(void *tortuga_ptr)
{
    Tortuga *tortuga = (Tortuga *)tortuga_ptr;

    Resultado *resultado = malloc (sizeof(Resultado));

    resultado->tiempo_descanso = tortuga->espera;
    resultado->pasos_periodo = tortuga->pasos;

    while(! *(tortuga->carrera_terminada))
    {
        sleep(tortuga->espera);
        sem_wait(tortuga->semaforo_ventanas);

        if ( *tortuga->carrera_terminada )
        {
            sem_post(tortuga->semaforo_ventanas);
            break;
        }

        wattrset(tortuga->ventana_mensajes, COLOR_PAIR(tortuga->color));
        wattrset(tortuga->ventana_carrera, COLOR_PAIR(tortuga->color));

        wprintw(tortuga->ventana_mensajes, "T%d: Avanzo %d pasos y duermo %d segundos\n", tortuga->numero, tortuga->pasos, tortuga->espera);

        mvwaddch(tortuga->ventana_carrera, tortuga->numero + 1, tortuga->pos, ' ');
        
        waddch(tortuga->ventana_carrera, ' ');

        tortuga->pos += tortuga->pasos;

        if (tortuga->pos >= COLUMNAS - 3)
        {
            *(tortuga->carrera_terminada) = 1;
            tortuga->pos = COLUMNAS - 2;
            wattron(tortuga->ventana_carrera, A_BOLD);
        }

        mvwadd_wch(tortuga->ventana_carrera, tortuga->numero + 1, tortuga->pos, &U1F422);

        wrefresh(tortuga->ventana_mensajes);
        wrefresh(tortuga->ventana_carrera);

        sem_post(tortuga->semaforo_ventanas); 
    }

    return resultado;
}

