#ifndef UART_H_
#define UART_H_

int uartIniciar();
void uartFechar();
void captaBotao_e1(unsigned char *dados);
void captaBotao_e2(unsigned char *dados);
int captaEncoder_e1();
int captaEncoder_e2();
int btnApagar_e1(int base);
int btnApagar_e2(int base);
int pwmEnviar_e1(int pwm);
int pwmEnviar_e2(int pwm);
void temperaturaEnviar_e1(float temperatura);
void temperaturaEnviar_e2(float temperatura);



#endif