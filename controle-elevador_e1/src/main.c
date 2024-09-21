#include <stdio.h>
#include <wiringPi.h>
#include <elevadorControle.h>
#include <filaEventos.h>
#include <uart.h>
#include <queue.h>
//#include <i2c.h>
#include <calibraSensores.h>


int main(){
    if (wiringPiSetup () == -1){
        return 1 ;
    }
    if (uartIniciar() == -1) {
        return -1; 
    }
    queue = createQueue();
   
    //calibraSensorAndar();

    e1_controlador();


    return 0;
}
