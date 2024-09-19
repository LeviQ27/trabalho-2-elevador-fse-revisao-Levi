#include "calibraSensores.h"
#include <stdio.h>

int posicao_Elevador1;
int sensor_E1[] = {0,0,0,0};

int posicao_Elevador2;
int sensor_E2[] = {0,0,0,0};


// Sensor do térreo
void sensorCallback0() {
    // Elevador 1
    int state1 = digitalRead(Sensor_Terreo_E1);
    static int value = 0; 
    if (state1 == HIGH) {
        printf("Sensor ativado - Elevador 1 chegou ao %d sensor.\nPosição: %d\n", 0, posicao_Elevador1);
        if(sensor_E1[0] == 0 ){
            sensor_E1[0] = posicao_Elevador1;
        }
    } else {
        printf("Sensor desativado - Elevador 1 saiu do %d sensor.\nPosição: %d\n", 0, posicao_Elevador1);
    }

    // Elevador 2
    int state2 = digitalRead(Sensor_Terreo_E2);
    if (state2 == HIGH) {
        printf("Sensor ativado - Elevador 2 chegou ao %d sensor.\nPosição: %d\n", 0, posicao_Elevador2);
        if(sensor_E2[0] == 0 ){
            sensor_E2[0] = posicao_Elevador2;
        }
    } else {
        printf("Sensor desativado - Elevador 2 saiu do %d sensor.\nPosição: %d\n", 0, posicao_Elevador2);
    }
}


// Sensor do 1 andar
void sensorCallback1() {

    // Elevador 1
    int state1 = digitalRead(Sensor_1_andar_E1);
    static int value = 0; 
    if (state1 == HIGH) {
        printf("Sensor ativado - Elevador 1 chegou ao %d sensor.\nPosição: %d\n", 1, posicao_Elevador1);
         if(sensor_E1[1] == 0 ){
            sensor_E1[1] = posicao_Elevador1;
        }
    } else {
        printf("Sensor desativado - Elevador 1 saiu do %d sensor.\nPosição: %d\n", 1, posicao_Elevador1);  
    }

    // Elevador 2
    int state2 = digitalRead(Sensor_1_andar_E2);
    if (state2 == HIGH) {
        printf("Sensor ativado - Elevador 2 chegou ao %d sensor.\nPosição: %d\n", 1, posicao_Elevador2);
         if(sensor_E2[1] == 0 ){
            sensor_E2[1] = posicao_Elevador2;
        }
    } else {
        printf("Sensor desativado - Elevador 2 saiu do %d sensor.\nPosição: %d\n", 1, posicao_Elevador2);  
    }
  
}


// Sensor do 2 andar
void sensorCallback2() {

    // Elevador 1
    int state1 = digitalRead(Sensor_2_andar_E1);
    static int value = 0;
    if (state1 == HIGH) {
        printf("Sensor ativado - Elevador 1 chegou ao %d sensor.\nPosição: %d\n", 2, posicao_Elevador1);
        if(sensor_E1[2] == 0 ){
            sensor_E1[2] = posicao_Elevador1;
        }
    } else {
        printf("Sensor desativado - Elevador 1 saiu do %d sensor.\nPosição: %d\n", 2, posicao_Elevador1);
    }

    // Elevador 2
    int state2 = digitalRead(Sensor_2_andar_E2);
    if (state2 == HIGH) {
        printf("Sensor ativado - Elevador 2 chegou ao %d sensor.\nPosição: %d\n", 2, posicao_Elevador2);
        if(sensor_E2[2] == 0 ){
            sensor_E2[2] = posicao_Elevador2;
        }
    } else {
        printf("Sensor desativado - Elevador 2 saiu do %d sensor.\nPosição: %d\n", 2, posicao_Elevador2);
    }
}


// Sensor do 3 andar
void sensorCallback3() {

    // Elevador 1
    int state1 = digitalRead(Sensor_3_andar_E1);
    static int value = 0;
    if (state1 == HIGH) {
        printf("Sensor ativado - Elevador 1 chegou ao %d sensor.\nPosição: %d\n", 3, posicao_Elevador1);
        if(sensor_E1[3] == 0 ){
            sensor_E1[3] = posicao_Elevador1;
        }
        
    } else {
        printf("Sensor desativado - Elevador 2 saiu do %d sensor.\nPosição: %d\n", 3, posicao_Elevador1);
    }

    // Elevador 2
    int state2 = digitalRead(Sensor_3_andar_E2);
    if (state2 == HIGH) {
        printf("Sensor ativado - Elevador 2 chegou ao %d sensor.\nPosição: %d\n", 3, posicao_Elevador2);
        if(sensor_E2[3] == 0 ){
            sensor_E2[3] = posicao_Elevador2;
        }
        
    } else {
        printf("Sensor desativado - Elevador 2 saiu do %d sensor.\nPosição: %d\n", 3, posicao_Elevador2);
    }
}



void* lerEncoder(void* arg) {
    while (1) {
        int posicao = captaEncoder_e1();
        if (posicao >= 0 && posicao <= 25500){
            posicao_Elevador1 = posicao;
        }
        
        posicao = captaEncoder_e2();
        if (posicao >= 0 && posicao <= 25500) {
            posicao_Elevador2 = posicao;
        }
    }

    return NULL;
}



// Função p calibrar os sensores e controlar os elevadores
void calibraSensorAndar(int elevador) {
    pthread_t threadEncoder;

    pthread_create(&threadEncoder, NULL, lerEncoder, &elevador);
    delay(200);

    if (elevador == 1) {
        pinMode(Sensor_Terreo_E1, INPUT);
        pinMode(Sensor_1_andar_E1, INPUT);
        pinMode(Sensor_2_andar_E1, INPUT);
        pinMode(Sensor_3_andar_E1, INPUT);
        pinMode(DIR1_E1, OUTPUT);
        pinMode(DIR2_E1, OUTPUT);
        pinMode(POTM_E1, OUTPUT);
        softPwmCreate(POTM_E1, 0, 100);

        pullUpDnControl(Sensor_Terreo_E1, PUD_UP);
        pullUpDnControl(Sensor_1_andar_E1, PUD_UP);
        pullUpDnControl(Sensor_2_andar_E1, PUD_UP);
        pullUpDnControl(Sensor_3_andar_E1, PUD_UP);

        wiringPiISR(Sensor_Terreo_E1, INT_EDGE_BOTH, &sensorCallback0);
        wiringPiISR(Sensor_1_andar_E1, INT_EDGE_BOTH, &sensorCallback1);
        wiringPiISR(Sensor_2_andar_E1, INT_EDGE_BOTH, &sensorCallback2);
        wiringPiISR(Sensor_3_andar_E1, INT_EDGE_BOTH, &sensorCallback3);
    }
    
    else if (elevador == 2) {
        pinMode(Sensor_Terreo_E2, INPUT);
        pinMode(Sensor_1_andar_E2, INPUT);
        pinMode(Sensor_2_andar_E2, INPUT);
        pinMode(Sensor_3_andar_E2, INPUT);
        pinMode(DIR1_E2, OUTPUT);
        pinMode(DIR2_E2, OUTPUT);
        pinMode(POTM_E2, OUTPUT);
        softPwmCreate(POTM_E2, 0, 100);

        pullUpDnControl(Sensor_Terreo_E2, PUD_UP);
        pullUpDnControl(Sensor_1_andar_E2, PUD_UP);
        pullUpDnControl(Sensor_2_andar_E2, PUD_UP);
        pullUpDnControl(Sensor_3_andar_E2, PUD_UP);

        wiringPiISR(Sensor_Terreo_E2, INT_EDGE_BOTH, &sensorCallback0);
        wiringPiISR(Sensor_1_andar_E2, INT_EDGE_BOTH, &sensorCallback1);
        wiringPiISR(Sensor_2_andar_E2, INT_EDGE_BOTH, &sensorCallback2);
        wiringPiISR(Sensor_3_andar_E2, INT_EDGE_BOTH, &sensorCallback3);
    }

    while (1) {
        if (elevador == 1){
            if (posicao_Elevador1 < 21691){
                digitalWrite(DIR1_E1, 1);
                digitalWrite(DIR2_E1, 1);
                softPwmWrite(POTM_E1, 3);
            }
            else if (posicao_Elevador1 > 21695) {
                digitalWrite(DIR1_E1, 0);
                digitalWrite(DIR2_E1, 1);
                softPwmWrite(POTM_E1, 3);
            }
            else {
                digitalWrite(DIR1_E1, 1);
                digitalWrite(DIR2_E1, 1);
            }
        }
        
        else if (elevador == 2) {
            if (posicao_Elevador2 < 21691) {
                digitalWrite(DIR1_E2, 1);
                digitalWrite(DIR2_E2, 1);
                softPwmWrite(POTM_E2, 3);
            }
            else if (posicao_Elevador2 > 21695) {
                digitalWrite(DIR1_E2, 0);
                digitalWrite(DIR2_E2, 1);
                softPwmWrite(POTM_E2, 3);
            }
            else {
                digitalWrite(DIR1_E2, 1);
                digitalWrite(DIR2_E2, 1);
            }
        }
    }

    // Imprime posicao dos sensores pos calibracao
    if (elevador == 1) {
        printf("Elevador 1 - Terminal: %d, Andar 1: %d, Andar 2: %d, Andar 3: %d\n", sensor_E1[0], sensor_E1[1], sensor_E1[2], sensor_E1[3]);
    } else if (elevador == 2) {
        printf("Elevador 2 - Terminal: %d, Andar 1: %d, Andar 2: %d, Andar 3: %d\n", sensor_E2[0], sensor_E2[1], sensor_E2[2], sensor_E2[3]);
    }
}