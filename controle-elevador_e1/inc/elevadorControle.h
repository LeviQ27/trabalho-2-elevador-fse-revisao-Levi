#ifndef ELEVADORCONTROLE_H_
#define ELEVADORCONTROLE_H_

#include <pthread.h>
#include <queue.h>

extern e1_andar[];
extern pthread_mutex_t e1_uartMutex; 

extern int requisicao_e1;
extern char estado[];

extern Queue* queue;

void e1_moverMotor(int referencia);
void configuraPinos();
void e1_controlador();
void e1_termina();


#endif 
