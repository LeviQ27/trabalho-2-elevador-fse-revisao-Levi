#ifndef CALIBRASENSORESE2_H
#define CALIBRASENSORESE2_H

#include <wiringPi.h>
#include <pthread.h>

#define E2_TER  0
#define E2_AND1  2
#define E2_AND2  3
#define E2_AND3  22
#define E2_DIR_1 24
#define E2_DIR_2 25
#define E2_POTM  23

void sen_e2_ElevadorCallback0();
void sen_e2_ElevadorCallback1();
void sen_e2_ElevadorCallback2();
void sen_e2_ElevadorCallback3();

void* e2_lerEncode(void* arg);
void e2_calibraSensorAndar();

#endif 