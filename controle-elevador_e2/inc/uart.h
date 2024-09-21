#ifndef UART_H_
#define UART_H_

int uartIniciar();
void uartFechar();


//Elevador 2
void e2_captaBotoes(unsigned char *dados);
int e2_captaEncoder();
void e2_botaoApaga(int base);
void e2_pwmEnvia(int pwm);
void e2_temperaturaEnvia(float temperatura);

#endif 
