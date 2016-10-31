#define _XOPEN_SOURCE_EXTENDED 1
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "tortuga.h"
#include "config.h"
#include "simbolos.h"

void *Tortuga_correr_hilo(void *tortuga_ptr)
{
    Tortuga *tortuga = (Tortuga *)tortuga_ptr;

    /* Alocamos memoria para el resultado */
    Resultado *resultado = malloc (sizeof(Resultado));

    /* Vamos guardando el tiempo de espera y el número de pasos */
    resultado->espera = tortuga->espera;
    resultado->pasos= tortuga->pasos;

    /* Guardamos la posición anterior */
    int pos_prev = -1;

    while(! *(tortuga->carrera_terminada))
    {
        /* Esperamos a que desocupen la consola */
        sem_wait(tortuga->semaforo_ventanas);

        /* Si hay una posición previa, la sobreescribimos con un espacio */
        if (pos_prev > 0)
        {
            mvwaddch(tortuga->ventana_carrera, tortuga->numero + 1, pos_prev, ' ');
        }

        /* Ajustamos los colores de la terminal a los de la tortuga */
        wattrset(tortuga->ventana_mensajes, COLOR_PAIR(tortuga->color));
        wattrset(tortuga->ventana_carrera, COLOR_PAIR(tortuga->color));

        /* Imprimimos la cantida de pasos y tiempo de espera */
        wprintw(tortuga->ventana_mensajes, "T%d: Avanzo %d pasos y duermo %d segundos\n", tortuga->numero, tortuga->pasos, tortuga->espera);

        /* Si ya se llegó a la meta */
        if (tortuga->pos >= TAMANO_PISTA - 2)
        {
            /* Marcamos que la carrera está terminada */
            *(tortuga->carrera_terminada) = TRUE;

            /* Ponemos nuestra posición al final */
            tortuga->pos = TAMANO_PISTA;

            /* Nos movemos allí */
            mvwadd_wch(tortuga->ventana_carrera, tortuga->numero + 1, tortuga->pos, &U2599);

            /* Actualizamos las ventanas y las liberamos*/
            wrefresh(tortuga->ventana_mensajes);
            wrefresh(tortuga->ventana_carrera);
            sem_post(tortuga->semaforo_ventanas);
            break;
        }
        

        /* Ponemos la tortuga en la posición adecuada */
        mvwadd_wch(tortuga->ventana_carrera, tortuga->numero + 1, tortuga->pos, &U2599);

        /* Actualizamos las ventanas */
        wrefresh(tortuga->ventana_mensajes);
        wrefresh(tortuga->ventana_carrera);

        /* Liberamos la ventana */
        sem_post(tortuga->semaforo_ventanas); 

        /* Calculamos la siguiente posición y dormimos */
        pos_prev = tortuga->pos;
        tortuga->pos += tortuga->pasos;

        sleep(tortuga->espera);
    }

    /* Almacenamos en el resultado la cantidad de pasos dados y
     * terminamos */
    resultado->pasos_total = tortuga->pos;

    return resultado;
}

