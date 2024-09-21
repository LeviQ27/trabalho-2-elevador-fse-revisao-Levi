#ifndef ELEVADORCONTROLEE2_H_
#define ELEVADORCONTROLEE2_H_

#include <pthread.h>
#include <queue.h>

extern e2_andar[];
extern pthread_mutex_t e2_uartMutex; 

extern int t_requisicao_e2;
extern char e2_estado[];

extern Queue* queue2;

void e2_moverMotor(int referencia);
void e2_configuraPinos();
void e2_controlador();
void e2_termina();

#endif 
