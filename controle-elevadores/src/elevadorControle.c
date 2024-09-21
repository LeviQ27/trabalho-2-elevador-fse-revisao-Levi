#include <unistd.h>  //Used for UART
#include <fcntl.h>   //Used for UART
#include <termios.h> //Used for UART
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <pthread.h>
#include <wiringPiI2C.h>

#include <bmp280.h>
#include <i2c.h>
#include <eventos.h>
#include <uart.h>
#include <fila.h>
#include <pid.h>
#include <crc.h>

// Elevador 1
#define DIR1_E1 20
#define DIR2_E1 21
#define POTM_E1 12

// Elevador 2
#define DIR1_E2 19
#define DIR2_E2 26
#define POTM_E2 13


//pthread_mutex_t uartMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t uartMutex_e1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t uartMutex_e2 = PTHREAD_MUTEX_INITIALIZER;

Fila* fila_1;
Fila* fila_2;
int andar_e1[] = { 2161 - 20, 6461 - 40, 12321 - 45, 21701 - 65};
int andar_e2[] = { 2161 - 20, 6461 - 40, 12321 - 45, 21701 - 65};

// térreo ->> 2151 - 2171
// 1° andar ->> 6451 - 6471
// 2° andar ->> 12311 - 12331
// 3° andar ->> 21691 - 21711

int requisicao_e1;
int requisicao_e2;
int posicaoElevador1 = 0;
int posicaoElevador2 = 0;
int termina = 0;
unsigned char dados[11];
char estado_e1[20] = "Parado";
char estado_e2[20] = "Parado";
int sinalPWM_1 = 0;
int sinalPWM_2 = 0;


void* lerBotoesThread(void* arg) {
    while (termina != 1) {
        fila1();
        fila2();
    } 
    return NULL;
}

void* lerEncoderThread(void* arg) {
    while (termina != 1) {
        int posicao = captaEncoder_e1();
        if(posicao >= 0 && posicao <= 25500){
            posicaoElevador1 = posicao;
        }

        usleep(150000);

        posicao = captaEncoder_e2();
        if(posicao >= 0 && posicao <= 25500){
            posicaoElevador2 = posicao;
        }

        usleep(150000);  
    }

    return NULL;
}


void* escreverPWMThread(void* arg) {
    while(termina != 1) {
        if(sinalPWM_1 < 0){
           pwmEnviar_e1(sinalPWM_1 * (-1)); 
        }else{
           pwmEnviar_e1(sinalPWM_1);
        }

        delay(1500);

        if (sinalPWM_2 < 0)
        {
            pwmEnviar_e2(sinalPWM_2 * (-1));
        }
        else
        {
            pwmEnviar_e2(sinalPWM_2);
        }

        delay(1500);
    }
    return NULL;
}

void pinosConfigura(){
    pinMode (DIR1_E1, OUTPUT); 
    pinMode (DIR2_E1, OUTPUT);
    pinMode(POTM_E1, OUTPUT);
    softPwmCreate(POTM_E1, 0, 100);

    pinMode (DIR1_E2, OUTPUT); 
    pinMode (DIR2_E2, OUTPUT);
    pinMode(POTM_E2, OUTPUT);
    softPwmCreate(POTM_E2, 0, 100);
}



void motorMovimento_e1(int referencia){
    
    pidConfigConstantes(0.005, 0.00002, 20);
    pidAtualizaReferencia((double) referencia );
    if (referencia > posicaoElevador1)
        {
            strcpy(estado_e1, "Subindo!");
            while(posicaoElevador1 <= referencia){
                int dado = posicaoElevador1;
                int potencia;
                if (dado >= 0 && dado < 25500)
                {
                    potencia = (int) pidControle((double) dado);
                    sinalPWM_1 = potencia;
                }
                if( potencia > 0){
                    // Elevador 1
                    digitalWrite(DIR1_E1, 1);
                    digitalWrite(DIR2_E1, 0);
                    softPwmWrite(POTM_E1, potencia);
                }
                else{
                    // Elevador 1
                    digitalWrite(DIR1_E1, 0);
                    digitalWrite(DIR2_E1, 1);
                    softPwmWrite(POTM_E1, (potencia * (-1)));
                }
            }
        }else{
            strcpy(estado_e1, "Descendo!");
            while(posicaoElevador1 >= referencia + 85){
                int dado = posicaoElevador1;
                int potencia;
                if (dado >= 0 && dado < 25500)
                {
                    potencia = (int) pidControle((double) dado);
                    sinalPWM_1 = potencia;

                }
                if( potencia < 0){
                    digitalWrite(DIR1_E1, 0);
                    digitalWrite(DIR2_E1, 1);
                    softPwmWrite(POTM_E1, (potencia * (-1)) );
                }else{
                    digitalWrite(DIR1_E1, 1);
                    digitalWrite(DIR2_E1, 0);
                    softPwmWrite(POTM_E1, potencia);
                }
            }
        }

    sinalPWM_1 = 0;
    strcpy(estado_e1, "Parado!");
    digitalWrite(DIR1_E1, 1);
    digitalWrite(DIR2_E1, 1);

    softPwmWrite(POTM_E1, 100);
    btnApagar_e1(requisicao_e1);
    retiraFila(fila_1);
    delay(10000);
}

void motorMovimento_e2(int referencia)
{
    pidConfigConstantes(0.005, 0.00002, 20);
    pidAtualizaReferencia((double)referencia);
    if (referencia > posicaoElevador2)
    {
        strcpy(estado_e2, "Subindo!");
        while (posicaoElevador2 <= referencia)
        {
            int dado = posicaoElevador2;
            int potencia;
            if (dado >= 0 && dado < 25500)
            {
                potencia = (int)pidControle((double)dado);
                sinalPWM_2 = potencia;
            }
            if (potencia > 0)
            {
                // Elevador 2
                digitalWrite(DIR1_E2, 1);
                digitalWrite(DIR2_E2, 0);
                softPwmWrite(POTM_E2, potencia);
            }
            else
            {
                // Elevador 2
                digitalWrite(DIR1_E2, 0);
                digitalWrite(DIR2_E2, 1);
                softPwmWrite(POTM_E2, (potencia * (-1)));
            }
        }
    }
    else
    {
        strcpy(estado_e2, "Descendo!");
        while (posicaoElevador2 >= referencia + 85)
        {
            int dado = posicaoElevador2;
            int potencia;
            if (dado >= 0 && dado < 25500)
            {
                potencia = (int)pidControle((double)dado);
                sinalPWM_2 = potencia;
            }
            if (potencia < 0)
            {
                digitalWrite(DIR1_E2, 0);
                digitalWrite(DIR2_E2, 1);
                softPwmWrite(POTM_E2, (potencia * (-1)));
            }
            else
            {
                digitalWrite(DIR1_E2, 1);
                digitalWrite(DIR2_E2, 0);
                softPwmWrite(POTM_E2, potencia);
            }
        }
    }

    sinalPWM_2 = 0;
    strcpy(estado_e2, "Parado");
    digitalWrite(DIR1_E2, 1);
    digitalWrite(DIR2_E2, 1);

    softPwmWrite(POTM_E2, 100);
    btnApagar_e2(requisicao_e2);
    retiraFila(fila_2);
    delay(10000);
}

int decodificaAndar(int i ){
    if(i == 0 || i == 7){
        return 0;
    }
    if(i == 1 || i == 2 || i == 8){
        return 1;
    }
    if(i == 3 || i == 4 || i == 9){
        return 2;
    }
    if(i == 5 || i == 10){
        return 3;
    }
    return -1;
}


void motorControle(){

    filaUnica(fila_1, 0);
    filaUnica(fila_2, 0);
    pthread_t threadBotoes, threadEncoder, threadPWM;

    pinosConfigura();

    pthread_create(&threadBotoes, NULL, lerBotoesThread, NULL);
    pthread_create(&threadEncoder, NULL, lerEncoderThread, NULL);
    pthread_create(&threadPWM, NULL, escreverPWMThread, NULL);

    delay(300);

    while(1){
        requisicao_e1 = visualizar(fila_1);
        if(requisicao_e1 != -1 ){
            delay(10);
            int posi = decodificaAndar(requisicao_e1);
            if(posi == -1){
                strcpy(estado_e1, "Emergencia!");
                digitalWrite(DIR1_E1, 1);
                digitalWrite(DIR2_E1, 1);
                
                digitalWrite(DIR1_E2, 1);
                digitalWrite(DIR2_E2, 1);

                btnApagar_e1(6);
                delay(1000);
                exit(0);
            }
            motorMovimento_e1(andar_e1[posi]);
        }

        requisicao_e2 = visualizar(fila_2);
        if (requisicao_e2 != -1)
        {
            delay(10);
            int posi = decodificaAndar(requisicao_e2);
            if (posi == -1)
            {
                strcpy(estado_e2, "Emergencia!");
                digitalWrite(DIR1_E1, 1);
                digitalWrite(DIR2_E1, 1);

                digitalWrite(DIR1_E2, 1);
                digitalWrite(DIR2_E2, 1);

                btnApagar_e2(6);
                delay(1000);
                exit(0);
            }
            motorMovimento_e2(andar_e2[posi]);
        }
    }
    termina = 1;
}