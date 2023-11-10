#include <stdio.h>
#include <stdlib.h>

#define TAM_LINEA 16
#define NUM_FILAS 8
#define TAM_RAM 4096


typedef struct { 
    unsigned char ETQ; 
    unsigned char Data[TAM_LINEA]; 
} T_CACHE_LINE; 




void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]); 
void inicializarLinea(T_CACHE_LINE *linea);
void VolcarCACHE(T_CACHE_LINE *tbl); 
int getRam(unsigned int acceso);
void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque); 
void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque); 
void MostrarCACHE(T_CACHE_LINE *tbl);
void lecturaArchivoBinario(char *nombreFichero, unsigned char *pSimul_RAM);


int main(int argc, char **argv){

    int globalTime=0, numFallos=0;
    T_CACHE_LINE cache[NUM_FILAS];
    unsigned char Simul_RAM[TAM_RAM];

    LimpiarCACHE(cache);
    MostrarCACHE(cache);

    lecturaArchivoBinario("CONTENTS_RAM.bin", Simul_RAM);

    FILE *accesos = NULL;
    accesos = fopen("accesos_memoria.txt", "r");
    
    if(accesos==NULL){
        printf("ERROR: archivo \"accesos_memoria.txt\" no encontrado\n");
        exit(-1);
    }

    //getRam(0x22E);

    return 0;
}

void inicializarLinea(T_CACHE_LINE *linea){
    linea->ETQ = 0xFF;
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

void lecturaArchivoBinario(char *nombreFichero, unsigned char *pSimul_RAM){
    FILE *arch = NULL;
    char bitLeido='\0';
    int ramOcupada=0 ;

    arch = fopen(nombreFichero, "rb");
    if (arch == NULL){
        printf("Error: el fichero %s no existe", nombreFichero);
        exit(1);
    }
        
    while((bitLeido = getc(arch)) != EOF || ramOcupada<TAM_RAM){
        pSimul_RAM[ramOcupada] = bitLeido;
        ramOcupada++;
    }

    fclose(arch);
}

int getRam(unsigned int acceso){
    int ETQ=0;
    int palabra=0;
    int linea=0;
    int bloque=0;
    ParsearDireccion(acceso,&ETQ,&palabra,&linea,&bloque);
    //printf("%d\n",ETQ);
    //printf("%d\n",palabra);
    //printf("%d\n",bloque);
    //parsea direccion
    //se busca
        //si fallo
        //se trata
        //se sale
    
        //si no hay fallo
        //se añade al fichero
        //Se sale
}

void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque){
    *ETQ = (addr & 0b111110000000) >> 7;
    *bloque = (addr & 0b000001110000) >> 4;
    *palabra = (addr & 0b000000001111);
    *linea = *bloque % NUM_FILAS;
}

