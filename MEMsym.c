#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>

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
int getRam(unsigned int acceso, T_CACHE_LINE* cache, char* texto, int* globalTime,  int *numFallos, unsigned char *ram, int numAcceso);
void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque); 
void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque); 
void MostrarCACHE(T_CACHE_LINE *tbl);
void lecturaArchivoBinario(FILE *arch, unsigned char *pSimul_RAM);
void imprimeEstadisticaDeAccesoYTiempo(int numAccesos, int tiempo, int fallos, char* texto);


int main(int argc, char **argv){

    int globalTime=0, numFallos=0;
    T_CACHE_LINE cache[NUM_FILAS];
    unsigned char Simul_RAM[TAM_RAM];
    char texto[100];
    FILE *arch = NULL;

    LimpiarCACHE(cache);
    MostrarCACHE(cache);

    arch = fopen("CONTENTS_RAM.bin","rb");

    if (arch == NULL){
        printf("Error: el fichero CONTENTS_RAM.bin no existe\n");
        return(-1);
    }

    lecturaArchivoBinario(arch, Simul_RAM);
    fclose(arch);

    FILE *accesos = NULL;
    accesos = fopen("accesos_memoria.txt", "r");
    
    if(accesos==NULL){
        printf("ERROR: archivo \"accesos_memoria.txt\" no encontrado\n");
        return (-1);
    }

    unsigned int direccion = 0;

    int numAccesos=0;
    while(fscanf(accesos,"%x", &direccion) != EOF){
        if(!getRam(direccion, cache, texto, &globalTime, &numFallos, Simul_RAM,numAccesos)){
            getRam(direccion, cache, texto, &globalTime, &numFallos, Simul_RAM,numAccesos);
        }
        numAccesos++;
        sleep(1);
    }

    imprimeEstadisticaDeAccesoYTiempo(numAccesos,globalTime,numFallos,texto);

    fclose(accesos);

    VolcarCACHE(cache);

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
        for(int j=TAM_LINEA-1; j>=0; j--)
            printf(" %x     ", tbl[i].Data[j]);
        printf("\n");
        
    }
}

void lecturaArchivoBinario(FILE *arch, unsigned char *pSimul_RAM){
    char bitLeido='\0';
    int ramOcupada=0 ;
        
    while((bitLeido = getc(arch)) != EOF || ramOcupada<TAM_RAM){
        pSimul_RAM[ramOcupada] = bitLeido;
        ramOcupada++;
    }
}

int getRam(unsigned int acceso, T_CACHE_LINE *cache, char* texto, int* globalTime, int *numFallos, unsigned char *ram, int numAcceso){
    int ETQ=0;
    int palabra=0;
    int linea=0;
    int bloque=0;
    int acierto = 0;
    
    ParsearDireccion(acceso,&ETQ,&palabra,&linea,&bloque);

    if((unsigned char)ETQ == cache[linea].ETQ){
        (*globalTime)++;
        printf("T: %d, Acierto de CACHE, ADDR %04X Label %X linea %02X palabra %02X DATO %02X\n",*globalTime,acceso,ETQ,linea,palabra,cache[linea].Data[palabra]);
        MostrarCACHE(cache);
        texto[numAcceso]=(char)cache[linea].Data[palabra];
        acierto = 1;
    }
    else{
        (*globalTime)+=20;
        (*numFallos)++;
        printf("T: %d, Fallo de CACHE %d, ADDR %04X Label %04X linea %02X palabra %02X bloque %02X", *globalTime, *numFallos, acceso, ETQ, linea, palabra, bloque);
        TratarFallo(cache, ram, ETQ, linea, bloque);
        printf("\nCargando el bloque %02X en la línea %02X", bloque, linea);
    }
    return acierto;
}

void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque){
    *ETQ = (addr & 0b111110000000) >> 7;
    *linea = (addr & 0b000001110000) >> 4;
    *palabra = (addr & 0b000000001111);
    *bloque = *ETQ * NUM_FILAS;
}

void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque){
    tbl[linea].ETQ = ETQ;
    for(int i=0; i<TAM_LINEA; i++){
        tbl[linea].Data[i] = (unsigned char) MRAM[TAM_LINEA*bloque + i];
    }
}

void imprimeEstadisticaDeAccesoYTiempo(int numAccesos, int tiempo, int fallos, char* texto){
    printf("Accesos totales: %d; fallos %d, Tiempo medio : %.2f\n",numAccesos,fallos,(float)tiempo/(float)numAccesos);
    printf("Texto leido: ");
    for(int i=0;i<numAccesos;i++){
        printf("%c",texto[i]);
    }
    printf("\n");
}


void VolcarCACHE(T_CACHE_LINE *tbl){
    FILE *arch =NULL;
    arch = fopen("CONTENTS_CACHE.bin", "wb");
    for(int i=0; i<NUM_FILAS; i++)
        fwrite(tbl[i].Data, sizeof(unsigned char), sizeof(tbl[i].Data), arch);
    fclose(arch);
}