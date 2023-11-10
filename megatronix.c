#include <stdio.h>
#include <stdlib.h>

#define TAM_LINEA 16
#define NUM_FILAS 8


typedef struct { 
unsigned char ETQ; 
unsigned char Data[TAM_LINEA]; 
} T_CACHE_LINE; 




void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]); 
void inicializarLinea(T_CACHE_LINE *linea);
void VolcarCACHE(T_CACHE_LINE *tbl); 
void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque); 
void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque); 
void MostrarCACHE(T_CACHE_LINE *tbl);



int main(int argc, char **argv){

    int globalTime=0, numFallos=0;
    T_CACHE_LINE cache[NUM_FILAS];
    LimpiarCACHE(cache);
    MostrarCACHE(cache);
    

}


void inicializarLinea(T_CACHE_LINE *linea){
    linea->ETQ = 0xff;
    for(int i=0; i<TAM_LINEA;i++){
        linea->Data[i] = 0x23;
    }
}

void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]){
    for(int i=0; i<NUM_FILAS; i++){
        inicializarLinea(&tbl[i]);
    }
}

void MostrarCACHE(T_CACHE_LINE *tbl){
    for (int i=0; i<NUM_FILAS; i++){
        printf("%x      Datos:", tbl[i].ETQ);
        for(int j=0; j<TAM_LINEA; j++)
            printf(" %x     ", tbl[i].Data[j]);
        printf("\n");
        
    }
}