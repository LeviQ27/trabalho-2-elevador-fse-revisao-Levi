#include "pid.h"
#include <stdio.h>


static double saidaMedida, sinalControle;
static double referencia = 0.0;

static double Kp = 0.5;  // Proporcional - valor sugerido
static double Ki = 0.05;  // Integral - valor sugerido
static double Kd = 40.0;  // Derivativo - valor sugerido
static int T = 1;      // Período de Amostragem (ms)

static unsigned long last_time;
static double erroTotal, erroAnterior = 0.0;
static int sinalControle_MAX = 100.0;
static int sinalControle_MIN = -100.0;


void pidConfigConstantes (double Kp_, double Ki_, double Kd_){
    Kp = Kp_;
    Ki = Ki_;
    Kd = Kd_;
}


void pidAtualizaReferencia (float referencia_){
    referencia = (double) referencia_;
}

double pidControle(double saidaMedida){
    double erro = referencia - saidaMedida;

    erroTotal += erro; // Integral - Vai acumulando o erro

    if (erroTotal >= sinalControle_MAX){
        erroTotal = sinalControle_MAX;
    }

    else if (erroTotal <= sinalControle_MIN){
        erroTotal = sinalControle_MIN;
    }


    double delta_error = erro - erroAnterior; // Derivativo - Diferença entre os erros
    
    sinalControle = Kp*erro + (Ki*T)*erroTotal + (Kd/T)*delta_error;
    
    if (sinalControle >= sinalControle_MAX){
        sinalControle = sinalControle_MAX;
    }
    
    else if (sinalControle <= sinalControle_MIN){
        sinalControle = sinalControle_MIN;
    }
    
    erroAnterior = erro;
    
    return sinalControle;
}