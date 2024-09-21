#include <stdio.h>
#include <elevadorControleE2.h>
#include <pthread.h>
#include <unistd.h>
#include <queue.h>
#include <uart.h>
#include <wiringPi.h>

unsigned char dadosBtn2[11];

void fila2()
{

    e2_captaBotoes(dadosBtn2);
    for (int i = 0; i < 11; i++)
    {
        if (dadosBtn2[i] > 1)
        {
            break;
        }
        if (dadosBtn2[i] == 1 && dadosBtn2[i] < 2)
        {
            enqueueUnique(queue2, i);
        }
    }
    delay(500);

}