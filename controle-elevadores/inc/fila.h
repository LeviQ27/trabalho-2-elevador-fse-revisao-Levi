#ifndef FILA_H // verifica se a FILA_H foi definida. Se não foi, roda o resto do código
#define FILA_H

#define MAX_SIZE 11

typedef struct {
    int items[MAX_SIZE];
    int frente;
    int posterior;
} Fila;


Fila* criaFila(); //fila.c

int eDuplicado(Fila* fila, int valor); //fila.c
void filaUnica(Fila* fila, int valor); //fila.c

int retiraFila(Fila* fila); //fila.c
void mostraFila(Fila* fila); //fila.c
int visualizar(Fila* fila) ; //fila.c

#endif