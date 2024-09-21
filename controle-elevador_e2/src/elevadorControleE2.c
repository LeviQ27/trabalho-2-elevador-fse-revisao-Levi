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

#include <calibraSensoresE2.h>

#define DIR_1_E2 24
#define DIR_2_E2 25
#define POTM_E2 23
#define SENSOR_T_ANDAR_E2 0
#define SENSOR_1_ANDAR_E2 2
#define SENSOR_2_ANDAR_E2 3
#define SENSOR_3_ANDAR_E2 22

pthread_mutex_t e2_uartMutex = PTHREAD_MUTEX_INITIALIZER;

Queue *queue2;
int e2_andar[] = {2161 - 20, 6461 - 40, 12321 - 45, 21701 - 65};

// térreo ->> 2151 - 2171
// 1° andar ->> 6451 - 6471
// 2° andar ->> 12311 - 12331
// 3° andar ->> 21691 - 21711

int t_requisicao_e2 = 0;
int posicao_e2_Elevador = 0;
int termina_e2 = 0;
unsigned char dadoElevador2[11];
char e2_estado[15] = "Parado";
int sinal_e2_PWM = 0;

void *e2_captaBotoesThread(void *arg)
{
    while (termina_e2 != 1)
    {
        fila2();
    }
    return NULL;
}

void grava_e2_encoder(int posicao)
{
    posicao_e2_Elevador = posicao;
}

void *e2_lerEncoder(void *args)
{
    while (termina_e2 != 1)
    {
        int posicao = e2_captaEncoder();
        usleep(150000);

        if (posicao >= 0 && posicao <= 25500)
            grava_e2_encoder(posicao);
    }

    return NULL;
}

void grava_e2_pwm(int pwm)
{
    sinal_e2_PWM = pwm;
}

void *e2_escreverPWMThread(void *arg)
{
    while (termina_e2 != 1)
    {
        if (sinal_e2_PWM < 0)
        {
            e2_pwmEnvia(sinal_e2_PWM * (-1));
        }
        else
        {
            e2_pwmEnvia(sinal_e2_PWM);
        }
        delay(100);
    }
    return NULL;
}

void e2_configuraPinos()
{
    pinMode(DIR_1_E2, OUTPUT);
    pinMode(DIR_2_E2, OUTPUT);
    pinMode(POTM_E2, OUTPUT);
    softPwmCreate(POTM_E2, 0, 100);
    pinMode(SENSOR_T_ANDAR_E2, INPUT);
    pinMode(SENSOR_1_ANDAR_E2, INPUT);
    pinMode(SENSOR_2_ANDAR_E2, INPUT);
    pinMode(SENSOR_3_ANDAR_E2, INPUT);
}

void acende_e2_sinal(int posicao)
{
    // térreo ->> 2151 - 2171
    // 1° andar ->> 6451 - 6471
    // 2° andar ->> 12311 - 12331
    // 3° andar ->> 21691 - 21711
    int t = (posicao >= 0 && posicao <= 2171);
    int a1 = (posicao >= 6451 && posicao <= 6471);
    int a2 = (posicao >= 12311 && posicao <= 12331);
    int a3 = (posicao >= 21691 && posicao <= 21711);

    digitalWrite(SENSOR_T_ANDAR_E2, t);
    digitalWrite(SENSOR_1_ANDAR_E2, a1);
    digitalWrite(SENSOR_2_ANDAR_E2, a2);
    digitalWrite(SENSOR_3_ANDAR_E2, a3);
    delay(100);
}

int decodifica_e2_andar(int i)
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

void estaciona_e2_elevador(int potencia, int requisicao)
{
    grava_e2_pwm(potencia);
    strcpy(e2_estado, "Parado");
    digitalWrite(DIR_1_E2, 1);
    digitalWrite(DIR_2_E2, 1);
    softPwmWrite(POTM_E2, 100);
    e2_botaoApaga(requisicao);
    delay(100);
    dequeue(queue2);
}

void *e2_moverMotor(void *args)
{
    while (termina_e2 != 1)
    {
        int referencia;

        int requisicao = peek(queue2);
        printf("Requisisao: %d\n", requisicao);

        if (requisicao != -1)
        {
            delay(20);
            int pos = decodifica_e2_andar(requisicao);
            if (pos == -1)
            {
                strcpy(e2_estado, "Emergencia");
                digitalWrite(DIR_1_E2, 1);
                digitalWrite(DIR_2_E2, 1);
                e2_botaoApaga(6);
                delay(100);
                break;
            }
            referencia = e2_andar[pos];
        }

        printf("Referencia: %d\n", referencia);
        printf("posicao_e2_Elevador: %d\n", posicao_e2_Elevador);

        pidConfigConst(0.005, 0.00002, 60);
        pidAtualizaReferencia((double)referencia);

        int potencia = 0;
        if (referencia > posicao_e2_Elevador && requisicao >= 0)
        {
            strcpy(e2_estado, "Subindo");
            while (posicao_e2_Elevador <= referencia)
            {
                potencia = (posicao_e2_Elevador == 0) ? 50 : (int)pidControl((double)posicao_e2_Elevador);
                printf("Potencia detro do if: %d\n", potencia);
                grava_e2_pwm(potencia);
                if (potencia > 0)
                {
                    digitalWrite(DIR_1_E2, 1);
                    digitalWrite(DIR_2_E2, 0);
                    softPwmWrite(POTM_E2, potencia);
                }
                else
                {
                    digitalWrite(DIR_1_E2, 1);
                    digitalWrite(DIR_2_E2, 0);
                    softPwmWrite(POTM_E2, (potencia * (-1)));
                }
                usleep(150000);
            }
            estaciona_e2_elevador(potencia, requisicao);
        }
        else if (referencia + 85 < posicao_e2_Elevador && requisicao >= 0)
        {
            strcpy(e2_estado, "Descendo");

            while (posicao_e2_Elevador >= referencia + 85)
            {
                potencia = (int)pidControl((double)posicao_e2_Elevador);
                grava_e2_pwm(potencia);
                if (potencia < 0)
                {
                    digitalWrite(DIR_1_E2, 0);
                    digitalWrite(DIR_2_E2, 1);
                    softPwmWrite(POTM_E2, (potencia * (-1)));
                }
                else
                {
                    digitalWrite(DIR_1_E2, 0);
                    digitalWrite(DIR_2_E2, 1);
                    softPwmWrite(POTM_E2, potencia);
                }
                usleep(150000);
            }
            estaciona_e2_elevador(potencia, requisicao);
        }
        else
            estaciona_e2_elevador(potencia, requisicao);
        
        acende_e2_sinal(posicao_e2_Elevador);
        printf("Potencia: %d\n", potencia);
        printf("%s\n", e2_estado);
        delay(500);
    }

    return NULL;
}

void e2_controlador()
{

    enqueueUnique(queue2, 0);
    pthread_t threadBotoes_e2, threadMotor_e2, threadLCD, threadPWM_e2, theadEncoder_e2;

    e2_configuraPinos();

    pthread_create(&threadBotoes_e2, NULL, e2_captaBotoesThread, NULL);
    // pthread_create(&threadLCD, NULL, LCD, NULL);
    pthread_create(&threadPWM_e2, NULL, e2_escreverPWMThread, NULL);
    pthread_create(&theadEncoder_e2, NULL, e2_lerEncoder, NULL);
    pthread_create(&threadMotor_e2, NULL, e2_moverMotor, NULL);

    delay(300);

    while (1)
    {
        if (t_requisicao_e2 != -1)
        {
            delay(20);
            int pos = decodifica_e2_andar(t_requisicao_e2);
            if (pos == -1)
            {
                strcpy(e2_estado, "Emergencia");
                digitalWrite(DIR_1_E2, 1);
                digitalWrite(DIR_2_E2, 1);
                e2_botaoApaga(6);
                delay(1000);
                break;
            }
        }
    }

    termina_e2 = 1;

    return 0;
}