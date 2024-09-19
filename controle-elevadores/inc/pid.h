#ifndef PID_H
#define PID_H


void pidConfigConstantes (double Kp_, double Ki_, double Kd_); //funcao p/ configurar os parametros do PID
void pidAtualizaReferencia (float referencia_); // funcao p/ atualizar o setpoint p/ o PID
double pidControle (double saidaMedida); // calcula a saida do PID

#endif