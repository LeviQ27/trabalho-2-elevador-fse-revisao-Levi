#ifndef ELEVADORCONTROLE_H
#define ELEVADORCONTROLE_H

#include <pthread.h>
#include <fila.h>

extern int andar_e1[];
extern int andar_e2[];
extern pthread_mutex_t uartMutex;

extern Fila* fila_1;
extern Fila* fila_2;

extern int termina;

extern int requisicao_e1;
extern int requisicao_e2;
extern int posicaoElevador1;
extern int posicaoElevador2;
extern int termina;
extern unsigned char dados[11];
extern char estado_e1[15];
extern char estado_e2[15];
extern int sinalPWM_1;
extern int sinalPWM_2;

void pinosConfigura();
void motorControle();

void *lerEncoderThread(void *arg);
void *escreverPWMThread(void *arg);
void *lerEncoderThread(void *arg);
void *lerBotoesThread(void *arg);

#endif