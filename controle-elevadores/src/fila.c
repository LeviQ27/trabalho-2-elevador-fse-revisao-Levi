#include "fila.h"
#include <stdio.h>
#include <stdlib.h>


Fila* criaFila() {
    Fila* fila = (Fila*)malloc(sizeof(Fila)*10);
    if (fila == NULL){
        fprintf(stderr, "Erro ao alocar memória para a fila!\n");
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; i < MAX_SIZE; i++){
        fila->items[i] = 0;  
    }
    
    fila->frente = -1;
    fila->posterior = -1;
    return fila;
}


int eDuplicado(Fila* fila, int valor){
    if (fila->frente == -1) {
        return 0; 
    }

    for (int i = fila->frente; i <= fila->posterior; i++){
        if (fila->items[i] == valor) {
            return 1; 
        }
    }
    return 0; 
}


void filaUnica(Fila* fila, int valor){
    if (fila->posterior == MAX_SIZE - 1) {
        fprintf(stderr, "Erro: A fila está cheia!\n");
        exit(EXIT_FAILURE);
    }

    if (!eDuplicado(fila, valor)){
        fila->posterior++;
        fila->items[fila->posterior] = valor;
        if (fila->frente == -1) {
            fila->frente = 0;
        }
    } 
}


int retiraFila(Fila* fila) {
    if (fila->frente == -1) {
        return -1;
    }

    int valor = fila->items[fila->frente];

    if (fila->frente == fila->posterior) {
        fila->frente = fila->posterior = -1;
    } else {
        fila->frente++;
    }

    return valor;
}


void mostraFila(Fila* fila) {
    if (fila->frente == -1) {
        return;
    }
    printf("Fila: ");
    for (int i = fila->frente; i <= fila->posterior; i++) {
        printf("%d ", fila->items[i]);
    }
    printf("\n |");
}


int visualizar(Fila* fila) {
    if (fila->frente == -1) {
        return -1;
    }
    return fila->items[fila->frente];
}