#include <stdio.h>
#include <elevadorControle.h>
#include <pthread.h>
#include <fila.h>
#include <uart.h>

unsigned char dadosBotao1[11];
unsigned char dadosBotao2[11];

void fila1() {

    captaBotao_e1(dadosBotao1);
    for (int i = 0; i < 11; i++) {
        if(dadosBotao1[i] > 1){
            break;
        }
        if (dadosBotao1[i] == 1 && dadosBotao1[i] < 2){
            filaUnica(fila_1, i);
        }
    }

    delay(50);
}

void fila2()
{

    captaBotao_e2(dadosBotao2);
    for (int i = 0; i < 11; i++)
    {
        if (dadosBotao2[i] > 1)
        {
            break;
        }
        if (dadosBotao2[i] == 1 && dadosBotao2[i] < 2)
        {
            filaUnica(fila_2, i);
        }
    }

    delay(50);
}