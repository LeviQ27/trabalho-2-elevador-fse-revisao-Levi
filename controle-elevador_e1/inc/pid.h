#ifndef PID_H_
#define PID_H_

void pidConfigConst(double Kp_, double Ki_, double Kd_);
void pidAtualizaReferencia(float referencia_);
double pidControl(double saida_medida);

#endif /* PID_H_ */