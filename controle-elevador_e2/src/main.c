#include <stdio.h>
#include <wiringPi.h>
#include <elevadorControleE2.h>
#include <filaEventos.h>
#include <uart.h>
#include <queue.h>
//#include <i2c.h>
#include <calibraSensoresE2.h>

int main(){
    if (wiringPiSetup () == -1){
        return 1 ;
    }
    if (uartIniciar() == -1) {
        return -1; 
    }
    queue2 = createQueue();
   
    //e2_calibraSensorAndar();

    e2_controlador();

    

    return 0;
}
