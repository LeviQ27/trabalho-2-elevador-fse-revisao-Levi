#include <stdio.h>
#include <elevadorControle.h>
#include <pthread.h>
#include <unistd.h>
#include <queue.h>
#include <uart.h>
#include <wiringPi.h>

unsigned char dadosBtn[11];

void fila() {

    e1_captaBotoes(dadosBtn);
    for (int i = 0; i < 11; i++) {
        if(dadosBtn[i] > 1){
            break;
        }
        if (dadosBtn[i] == 1 && dadosBtn[i] < 2){
            enqueueUnique(queue, i);
        }
    }
    delay(500);

}