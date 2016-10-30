#ifndef _TORTUGA_H
#define _TORTUGA_H

#define _XOPEN_SOURCE_EXTENDED 1
#include <ncurses.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct _Tortuga Tortuga;
typedef struct _Resultado Resultado;

struct _Tortuga {
    int numero;
    int pos;
    int pasos; // Cantidad de pasos que da por ciclo
    int espera; // Cantidad de tiempo que espera
    int *carrera_terminada;
    short color;
    WINDOW *ventana_mensajes;
    WINDOW *ventana_carrera;
    sem_t *semaforo_ventanas;
};

struct _Resultado {
    int pasos_dados;
    int pasos_periodo;
    int tiempo_descanso;

};

int Tortuga_correr(Tortuga *tortuga, pthread_t *hilo_ptr);

#endif
