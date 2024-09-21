#include "calibraSensoresE2.h"
#include "uart.h"

int pos_e2_Elevador;
int sen_e2_Elevador[] = {0,0,0,0};
void sen_e2_ElevadorCallback0() {

    int state = digitalRead(E2_TER);
    static int value = 0; 
    if (state == HIGH) {
        printf("sen_e2_Elevador ativado (Elevador chegou ao %d sen_e2_Elevador)\nposicao: %d\n", 0, pos_e2_Elevador);
        if(sen_e2_Elevador[0] == 0 ){
            sen_e2_Elevador[0] = pos_e2_Elevador;
        }
    } else {
        printf("sen_e2_Elevador desativado (Elevador saiu do %d sen_e2_Elevador)\nposicao: %d\n", 0, pos_e2_Elevador);
    }
}

void sen_e2_ElevadorCallback1( ) {

    int state = digitalRead(E2_AND1);
    static int value = 0; 
    if (state == HIGH) {
        printf("sen_e2_Elevador ativado (Elevador chegou ao %d sen_e2_Elevador)\nposicao: %d\n", 1, pos_e2_Elevador);
         if(sen_e2_Elevador[1] == 0 ){
            sen_e2_Elevador[1] = pos_e2_Elevador;
        }
    } else {
        printf("sen_e2_Elevador desativado (Elevador saiu do %d sen_e2_Elevador)\nposicao: %d\n", 1, pos_e2_Elevador);  
    }
  
}

void sen_e2_ElevadorCallback2( ) {

    int state = digitalRead(E2_AND2);

    if (state == HIGH) {
        printf("sen_e2_Elevador ativado (Elevador chegou ao %d sen_e2_Elevador)\nposicao: %d\n", 2, pos_e2_Elevador);
        if(sen_e2_Elevador[2] == 0 ){
            sen_e2_Elevador[2] = pos_e2_Elevador;
        }
    } else {
        printf("sen_e2_Elevador desativado (Elevador saiu do %d sen_e2_Elevador)\nposicao: %d\n", 2, pos_e2_Elevador);
    }
}

void sen_e2_ElevadorCallback3( ) {

    int state = digitalRead(E2_AND3);

    if (state == HIGH) {
        printf("sen_e2_Elevador ativado (Elevador chegou ao %d sen_e2_Elevador)\nposicao: %d\n", 3, pos_e2_Elevador);
        if(sen_e2_Elevador[3] == 0 ){
            sen_e2_Elevador[3] = pos_e2_Elevador;
        }
        
    } else {
        printf("sen_e2_Elevador desativado (Elevador saiu do %d sen_e2_Elevador)\nposicao: %d\n", 3, pos_e2_Elevador);
    }
}

void* e2_lerEncode(void* arg) {
    while (1) {
        int posicao = e2_captaEncoder();
        if(posicao >= 0 && posicao <= 25500){
            pos_e2_Elevador = posicao;
        } 
    }

    return NULL;
}

void e2_calibraSensorAndar()
{

    pthread_t  threadEncoder;

    pthread_create(&threadEncoder, NULL, e2_lerEncode, NULL);
    delay(200);
    pinMode(E2_TER, INPUT);
    pinMode(E2_AND1, INPUT);
    pinMode(E2_AND2, INPUT);
    pinMode(E2_AND3, INPUT);
    pinMode (E2_DIR_1, OUTPUT); 
    pinMode (E2_DIR_2, OUTPUT);
    pinMode(E2_POTM, OUTPUT);
    softPwmCreate(E2_POTM, 0, 100);

    pullUpDnControl(E2_TER, PUD_UP);
    pullUpDnControl(E2_AND1, PUD_UP);
    pullUpDnControl(E2_AND2, PUD_UP);
    pullUpDnControl(E2_AND3, PUD_UP);

    wiringPiISR(E2_TER, INT_EDGE_BOTH, &sen_e2_ElevadorCallback0);
    wiringPiISR(E2_AND1, INT_EDGE_BOTH, &sen_e2_ElevadorCallback1);
    wiringPiISR(E2_AND2, INT_EDGE_BOTH, &sen_e2_ElevadorCallback2);
    wiringPiISR(E2_AND3, INT_EDGE_BOTH, &sen_e2_ElevadorCallback3);
    while(1){
        if(pos_e2_Elevador < 21691){
            digitalWrite(E2_DIR_1, 1);
            digitalWrite(E2_DIR_2, 0);
            softPwmWrite(E2_POTM, 3);
        }else if(pos_e2_Elevador > 21695){
            digitalWrite(E2_DIR_1, 0);
            digitalWrite(E2_DIR_2, 1);
            softPwmWrite(E2_POTM, 3);
        }else{
            digitalWrite(E2_DIR_1, 1);
            digitalWrite(E2_DIR_2, 1);

        }

    }
    // -----pegar valor dos sen_e2_Elevadores
    /*
    while(pos_e2_Elevador < 25000){
        digitalWrite(E2_DIR_1, 1);
        digitalWrite(E2_DIR_2, 0);
        softPwmWrite(E2_POTM, 5);
    }*/

    digitalWrite(E2_DIR_1, 1);
    digitalWrite(E2_DIR_2,1);


    printf("0 - %d\n", sen_e2_Elevador[0]);
    printf("1 - %d\n", sen_e2_Elevador[1]);
    printf("2 - %d\n", sen_e2_Elevador[2]);
    printf("3 - %d\n", sen_e2_Elevador[3]);
}