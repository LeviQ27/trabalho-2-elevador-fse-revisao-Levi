#ifndef CALIBRASENSORES_H
#define CALIBRASENSORES_H

#include <wiringPi.h>
#include <pthread.h>


// Sensores do elevador 1
#define DIR1_E1 20
#define DIR2_E1 21
#define POTM_E1 12
#define Sensor_Terreo_E1 18
#define Sensor_1_andar_E1 23
#define Sensor_2_andar_E1 24
#define Sensor_3_andar_E1 25


// Sensores do elevador 2
#define DIR1_E2 19
#define DIR2_E2 26
#define POTM_E2 13
#define Sensor_Terreo_E2 17
#define Sensor_1_andar_E2 27
#define Sensor_2_andar_E2 22
#define Sensor_3_andar_E2 6


void sensorCallback0();
void sensorCallback1();
void sensorCallback2();
void sensorCallback3();




// Funções para leitura e calibração dos sensores
void* lerEncoder(void* arg);
void calibraSensorAndar(int elevador);

#endif 