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
int getData(unsigned int acceso, T_CACHE_LINE* cache, char* texto, int* globalTime,  int *numFallos, unsigned char *ram, int numAcceso);
void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque); 
void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque); 
void MostrarCACHE(T_CACHE_LINE *tbl);
void lecturaArchivoBinario(FILE *arch, unsigned char *pSimul_RAM);
void imprimeEstadisticaDeAccesoYTiempo(int numAccesos, int tiempo, int fallos, char* texto);


int main(int argc, char **argv){

    int globalTime=0, numFallos=0, numAccesos=0;
    T_CACHE_LINE cache[NUM_FILAS];
    unsigned char Simul_RAM[TAM_RAM];
    char texto[100];
    FILE *arch = NULL;
    FILE *accesos = NULL;
    unsigned int direccion = 0;


    //Inicializa la caché y la muestra por consola
    LimpiarCACHE(cache);
    MostrarCACHE(cache);

    
    arch = fopen("CONTENTS_RAM.bin","rb");
    //Comprobación de que el fichero binario del contenido de la RAM existe, si no informa de que falta y el programa devuelve -1
    if (arch == NULL){
        printf("Error: el fichero CONTENTS_RAM.bin no existe\n");
        return(-1);
    }
    //lectura del archivo y copia su contenido en Simul_RAM
    lecturaArchivoBinario(arch, Simul_RAM);
    fclose(arch);

    
    accesos = fopen("accesos_memoria.txt", "r");
    //Comprobación de que el fichero con las direcciones existe, si no informa de que falta y el programa devuelve -1
    if(accesos==NULL){
        printf("ERROR: archivo \"accesos_memoria.txt\" no encontrado\n");
        return (-1);
    }

    //Para cada acceso leido del archivo (mientras no se llegue al final del archivo)
    while(fscanf(accesos,"%x", &direccion) != EOF){
        //se obtiene el dato de la dirección leida 
        /*como getData devuelve si se ha producido acierto o no, 
        si se ha producido un fallo se vuelve a llamar a la función, pues esta vez el bloque ya estará cargado en la caché */
        if(!getData(direccion, cache, texto, &globalTime, &numFallos, Simul_RAM,numAccesos)){
            getData(direccion, cache, texto, &globalTime, &numFallos, Simul_RAM,numAccesos);
        }
        numAccesos++;
        sleep(1);
    }

    //se muestran las estadísticas sobre los accesos y el tiempo total 
    imprimeEstadisticaDeAccesoYTiempo(numAccesos,globalTime,numFallos,texto);

    fclose(accesos);

    VolcarCACHE(cache);

    return 0;
}


//Pone en la caché los valores por defecto: las etiquetas a 0xFF y las palabras de cada línea a 0x23
void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]){
    for(int i=0; i<NUM_FILAS; i++){
        inicializarLinea(&tbl[i]);
    }
}
void inicializarLinea(T_CACHE_LINE *linea){
    linea->ETQ = 0xFF;
    for(int i=0; i<TAM_LINEA;i++){
        linea->Data[i] = 0x23;
    }
}

//Muestra en la consola los datos almacenados en la caché 
void MostrarCACHE(T_CACHE_LINE *tbl){
    //para cada fila: imprime la etiqueta y las palabras de cada línea
    for (int i=0; i<NUM_FILAS; i++){
        printf("%x      Datos:", tbl[i].ETQ);
        for(int j=TAM_LINEA-1; j>=0; j--)
            printf(" %x     ", tbl[i].Data[j]);
        printf("\n");
        
    }
}

//Lectura del archivo binario que contiene la RAM 
void lecturaArchivoBinario(FILE *arch, unsigned char *pSimul_RAM){
    char bitLeido='\0';
    int ramOcupada=0 ;
    
    //lectura bit a bit del archivo binario y se guarda en el array Simul_RAM (también se comprueba que no se excede de la RAM máxima)
    while((bitLeido = getc(arch)) != EOF || ramOcupada<TAM_RAM){
        pSimul_RAM[ramOcupada] = bitLeido;
        ramOcupada++;
    }
}

//Dada una dirección leida del archivo de accesos, comprueba si está el dato solicitado en caché
// -si está, accede al dato 
// -si no está, trata el fallo
int getData(unsigned int acceso, T_CACHE_LINE *cache, char* texto, int* globalTime, int *numFallos, unsigned char *ram, int numAcceso){
    int ETQ=0;
    int palabra=0;
    int linea=0;
    int bloque=0;
    int acierto = 0;
    
    //parsea la dirección, obtiene los 3 campos de la dirección y el bloque de la RAM del que se desea sacar el dato
    ParsearDireccion(acceso,&ETQ,&palabra,&linea,&bloque);
    //comprueba si está cargado el bloque --> comprueba que en la línea indicada en la dirección se encuentra la etiqueta de la dirección
    if((unsigned char)ETQ == cache[linea].ETQ){
        (*globalTime)++;
        printf("T: %d, Acierto de CACHE, ADDR %04X Label %X linea %02X palabra %02X DATO %02X\n",*globalTime,acceso,ETQ,linea,palabra,cache[linea].Data[palabra]);
        MostrarCACHE(cache);
        texto[numAcceso]=(char)cache[linea].Data[palabra];//añade al array texto la palabra guardada en la dirección correspondiente
        acierto = 1;
    }
    //si se produce un fallo --> el bloque no está cargado en la caché 
    else{
        //se incrementa el tiempo sumandole la penalización y al nº de fallos se suma 1
        (*globalTime)+=20;
        (*numFallos)++;
        printf("T: %d, Fallo de CACHE %d, ADDR %04X Label %04X linea %02X palabra %02X bloque %02X", *globalTime, *numFallos, acceso, ETQ, linea, palabra, bloque);
        //Se carga en la caché el bloque del que se necesita extraer el dato y se informa del proceso 
        TratarFallo(cache, ram, ETQ, linea, bloque);
        printf("\nCargando el bloque %02X en la línea %02X", bloque, linea);
    }
    //la función devuelve si se ha producido un acierto (1) o un fallo (0)
    return acierto;
}

//Pasada una dirección, la divide en los campos etiqueta, línea y palabra (campos de la correspondencia directa)
void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque){
    /*Se sacan los 3 campos que tiene la dirección según la correspondencia directa, 
    para ello empleamos una máscara de bits y los desplazamos a la derecha las posiciones correspondientes para obtener el número al que cada parte hace referencia*/
    *ETQ = (addr & 0b111110000000) >> 7;
    *linea = (addr & 0b000001110000) >> 4;
    *palabra = (addr & 0b000000001111);
    /*El nº de bloque de la RAM que es referenciado en la dirección se saca multiplicando el valor de la etiqueta por el número de líneas de la caché,
    pues la etiqueta nos indica qué bloque de los que se pueden instanciar en una determinada línea es
    */
    *bloque = *ETQ * NUM_FILAS; 
}

//Cuando en la caché, la línea de la dirección parseada no tiene instanciado el bloque solicitado de la RAM, se produce un fallo
//Carga en la caché el bloque solicitado en la línea correspondiente
void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque){
    tbl[linea].ETQ = ETQ; //cambia la etiqueta de la línea de la RAM (indicando qué bloque está cargado)
    //copia los datos del bloque en la línea correspondiente de la caché 
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

//Vuelca (copia) los datos cargados en la caché en un fichero binario
void VolcarCACHE(T_CACHE_LINE *tbl){
    FILE *arch =NULL;
    //apertura del fichero CONTENTS_CACHE.bin en modo escritura binaria
    arch = fopen("CONTENTS_CACHE.bin", "wb");
    //para cada línea de la caché se escriben los datos al final del archivo 
    for(int i=0; i<NUM_FILAS; i++)
        fwrite(tbl[i].Data, sizeof(unsigned char), sizeof(tbl[i].Data), arch);
    fclose(arch);
}