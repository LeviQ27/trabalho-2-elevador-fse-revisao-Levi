#ifndef UART_H_
#define UART_H_

int uartIniciar();
void uartFechar();

//Elevador 1
void e1_captaBotoes(unsigned char *dados);
int e1_captaEncoder();
void e1_botaoApaga(int base);
void e1_pwmEnvia(int pwm);
void e1_temperaturaEnvia(float temperatura);

#endif 
