#include <unistd.h>  //Used for UART
#include <fcntl.h>   //Used for UART
#include <termios.h> //Used for UART
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <pthread.h>
#include <wiringPiI2C.h>

#include <filaEventos.h>
#include <uart.h>
#include <queue.h>
#include <pid.h>
#include <crc16.h>

#define DIR_1 28
#define DIR_2 29
#define POTM 26
#define SENSOR_T_ANDAR 1
#define SENSOR_1_ANDAR 4
#define SENSOR_2_ANDAR 5
#define SENSOR_3_ANDAR 6

pthread_mutex_t e1_uartMutex = PTHREAD_MUTEX_INITIALIZER;

Queue *queue;
int e1_andar[] = {2161 - 20, 6461 - 40, 12321 - 45, 21701 - 65};

// térreo ->> 2151 - 2171
// 1° andar ->> 6451 - 6471
// 2° andar ->> 12311 - 12331
// 3° andar ->> 21691 - 21711

int requisicao_e1 = 0;
int posicaoElevador = 0;
int termina = 0;
unsigned char dados[11];
char estado[15] = "Parado";
int sinalPWM = 0;

void *e1_captaBotoesThread(void *arg)
{
    while (termina != 1)
    {
        fila();
    }
    return NULL;
}

void grava_encoder(int posicao)
{
    posicaoElevador = posicao;
}

void *e1_lerEncoder(void *args)
{
    while (termina != 1)
    {
        int posicao = e1_captaEncoder();
        usleep(150000);

        if (posicao >= 0 && posicao <= 25500)
            grava_encoder(posicao);
    }

    return NULL;
}

void grava_pwm(int pwm)
{
    sinalPWM = pwm;
}

void *e1_escreverPWMThread(void *arg)
{
    while (termina != 1)
    {
        if (sinalPWM < 0)
        {
            e1_pwmEnvia(sinalPWM * (-1));
        }
        else
        {
            e1_pwmEnvia(sinalPWM);
        }
        delay(100);
    }
    return NULL;
}

void e1_configuraPinos()
{
    pinMode(DIR_1, OUTPUT);
    pinMode(DIR_2, OUTPUT);
    pinMode(POTM, OUTPUT);
    softPwmCreate(POTM, 0, 100);
    pinMode(SENSOR_T_ANDAR, INPUT);
    pinMode(SENSOR_1_ANDAR, INPUT);
    pinMode(SENSOR_2_ANDAR, INPUT);
    pinMode(SENSOR_3_ANDAR, INPUT);
}

void acende_sinal(int posicao)
{
    // térreo ->> 2151 - 2171
    // 1° andar ->> 6451 - 6471
    // 2° andar ->> 12311 - 12331
    // 3° andar ->> 21691 - 21711
    int t = (posicao >= 0 && posicao <= 2171);
    int a1 = (posicao >= 6451 && posicao <= 6471);
    int a2 = (posicao >= 12311 && posicao <= 12331);
    int a3 = (posicao >= 21691 && posicao <= 21711);

    digitalWrite(SENSOR_T_ANDAR, t);
    digitalWrite(SENSOR_1_ANDAR, a1);
    digitalWrite(SENSOR_2_ANDAR, a2);
    digitalWrite(SENSOR_3_ANDAR, a3);
    delay(100);
}

int decodifica_e1_andar(int i)
{
    if (i == 0 || i == 7)
    {
        return 0;
    }
    if (i == 1 || i == 2 || i == 8)
    {
        return 1;
    }
    if (i == 3 || i == 4 || i == 9)
    {
        return 2;
    }
    if (i == 5 || i == 10)
    {
        return 3;
    }
    return -1;
}

void estaciona_elevador(int potencia, int requisicao)
{
    grava_pwm(potencia);
    strcpy(estado, "Parado");
    digitalWrite(DIR_1, 1);
    digitalWrite(DIR_2, 1);
    softPwmWrite(POTM, 100);
    e1_botaoApaga(requisicao);
    delay(100);
    dequeue(queue);
}

void *e1_moverMotor(void *args)
{
    while (termina != 1)
    {
        int referencia;

        int requisicao = peek(queue);
        printf("Requisisao: %d\n", requisicao);

        if (requisicao != -1)
        {
            delay(20);
            int pos = decodifica_e1_andar(requisicao);
            if (pos == -1)
            {
                strcpy(estado, "Emergencia");
                digitalWrite(DIR_1, 1);
                digitalWrite(DIR_2, 1);
                e1_botaoApaga(6);
                delay(100);
                break;
            }
            referencia = e1_andar[pos];
        }

        printf("Referencia: %d\n", referencia);
        printf("posicaoElevador: %d\n", posicaoElevador);

        pidConfigConst(0.005, 0.00002, 60);
        pidAtualizaReferencia((double)referencia);

        int potencia = 0;
        if (referencia > posicaoElevador && requisicao >= 0)
        {
            strcpy(estado, "Subindo");
            while (posicaoElevador <= referencia)
            {
                potencia = (posicaoElevador == 0) ? 50 : (int)pidControl((double)posicaoElevador);
                printf("Potencia detro do if: %d\n", potencia);
                grava_pwm(potencia);
                if (potencia > 0)
                {
                    digitalWrite(DIR_1, 1);
                    digitalWrite(DIR_2, 0);
                    softPwmWrite(POTM, potencia);
                }
                else
                {
                    digitalWrite(DIR_1, 1);
                    digitalWrite(DIR_2, 0);
                    softPwmWrite(POTM, (potencia * (-1)));
                }
                usleep(150000);
            }
            estaciona_elevador(potencia, requisicao);
        }
        else if (referencia + 85 < posicaoElevador && requisicao >= 0)
        {
            strcpy(estado, "Descendo");

            while (posicaoElevador >= referencia + 85)
            {
                potencia = (int)pidControl((double)posicaoElevador);
                grava_pwm(potencia);
                if (potencia < 0)
                {
                    digitalWrite(DIR_1, 0);
                    digitalWrite(DIR_2, 1);
                    softPwmWrite(POTM, (potencia * (-1)));
                }
                else
                {
                    digitalWrite(DIR_1, 0);
                    digitalWrite(DIR_2, 1);
                    softPwmWrite(POTM, potencia);
                }
                usleep(150000);
            }
            estaciona_elevador(potencia, requisicao);
        }
        else
            estaciona_elevador(potencia, requisicao);

        acende_sinal(posicaoElevador);
        printf("Potencia: %d\n", potencia);
        printf("%s\n", estado);
        delay(500);
    }

    return NULL;
}

void e1_controlador()
{
    enqueueUnique(queue, 0);
    pthread_t threadBotoes_e1, threadMotor_e1, threadLCD, threadPWM_e1, theadEncoder_e1;

    e1_configuraPinos();

    pthread_create(&threadBotoes_e1, NULL, e1_captaBotoesThread, NULL);
    // pthread_create(&threadLCD, NULL, LCD, NULL);
    pthread_create(&threadPWM_e1, NULL, e1_escreverPWMThread, NULL);
    pthread_create(&theadEncoder_e1, NULL, e1_lerEncoder, NULL);
    pthread_create(&threadMotor_e1, NULL, e1_moverMotor, NULL);

    delay(300);

    while (1)
    {
        if (requisicao_e1 != -1)
        {
            delay(20);
            int pos = decodifica_e1_andar(requisicao_e1);
            if (pos == -1)
            {
                strcpy(estado, "Emergencia");
                digitalWrite(DIR_1, 1);
                digitalWrite(DIR_2, 1);
                e1_botaoApaga(6);
                delay(1000);
                break;
            }
        }
    }

    termina = 1;

    return 0;
}