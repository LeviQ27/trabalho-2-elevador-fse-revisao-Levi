#include "pid.h"
#include <stdio.h>

static double saida_medida, sinal_de_controle;
static double referencia_pid = 0.0;
static double Kp = 0.0;  // Ganho Proporcional
static double Ki = 0.0;  // Ganho Integral
static double Kd = 0.0;  // Ganho Derivativo
static int T = 1;      // Período de Amostragem (ms)
static unsigned long last_time;
static double erro_total, erro_anterior = 0.0;
static int sinal_de_controle_MAX = 100.0;
static int sinal_de_controle_MIN = -100.0;

void pidConfigConst(double Kp_, double Ki_, double Kd_){
    Kp = Kp_;
    Ki = Ki_;
    Kd = Kd_;
}

void pidAtualizaReferencia(float referencia_){
    referencia_pid = (double) referencia_;
}

double pidControl(double saida_medida){
    double erro = referencia_pid - saida_medida;

    erro_total += erro; // Acumula o erro (Termo Integral)

    if (erro_total >= sinal_de_controle_MAX) 
    {
        erro_total = sinal_de_controle_MAX;
    }
    else if (erro_total <= sinal_de_controle_MIN) 
    {
        erro_total = sinal_de_controle_MIN;
    }

    double delta_error = erro - erro_anterior; // Diferença entre os erros (Termo Derivativo)
    sinal_de_controle = Kp*erro + (Ki*T)*erro_total + (Kd/T)*delta_error; // PID calcula sinal de controle
    if (sinal_de_controle >= sinal_de_controle_MAX) 
    {
        sinal_de_controle = sinal_de_controle_MAX;
    }
    else if (sinal_de_controle <= sinal_de_controle_MIN) 
    {
        sinal_de_controle = sinal_de_controle_MIN;
    }
    erro_anterior = erro;
    
    return sinal_de_controle;
}
