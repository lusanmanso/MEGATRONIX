#include <stdio.h>
#include <stdlib.h>

//inicialmente defino estas variables aqui, aunque al desarrollar el código puede que no hagan falta o sirvan, o que tengan que estar definidas en main
#define TAM_LINEA 8
#define NUM_FILAS 0
#define TAM_PALABRA 4
#define TAM_ETQ 5

//en pdf, NO CAMBIAR
typedef struct{
    unsigned char ETQ;
    unsigned char Data[TAM_LINEA];
}T_CACHE_LINE;

//funciones OBLIGATORIAS
void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]);
void VolcarCACHE(T_CACHE_LINE *tbl);
void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque);
void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque);

//funciones extra
int contarLineas(FILE *fd);
unsigned int *leerLinea(FILE *fd, int *addr);
void imprimirCACHE(T_CACHE_LINE *tbl);

int main(){
    unsigned int addr;
    int *ETQ;
    int *palabra;
    int *linea;
    int *bloque;

    //deben ser unsigned?
    int globaltime = 0;
    int numfallos = 0;

    T_CACHE_LINE *tbl; //hay que definir tamaño 8

    FILE *fd;

    LimpiarCACHE(tbl); //asigna xFF a label y x23 a datos cache
    imprimirCACHE(tbl);  //imprime cache para visualizar antes de comenzar el código, remover en código final

    /*ANTES DEL RESTO DEL CÓDIGO SE DEBERIA LEER CONTENTS_RAM.bin
    no entiendo al 100% que se pide cuando dice "lee el fichero binario CONTENTS_RAM.bin **en la variable Simul_RAM**", 
    ¿se refiere a que hay que almacenar lo leido en el fichero binario en Simul_RAM?*/

    //todo lo siguiente deberia estar dentro del else de la lectura de CONTENTS_RAM.bin (se ejecuta solo si existe el fichero CONTENTS_RAM.bin)
    fd = fopen("dirs_memoria.txt", "r"); //abrir fichero dirs_memoria.txt para leerlo
    if(fd==NULL){
        printf("Error. Archivo dirs_memoria.txt no existe.\n\n");
        return -1;
    }else{
        printf("Acceso a dirs_memoria.txt correcto.\n\n");
        for(int i=0; i<contarLineas(fd); i++){
            //leer linea
            //comparar etiquetas
            //if(iguales)
                //printf("T: %d, Acierto de CACHE, ADDR %04X Label %X linea %02X palabra %02X DATO %02X\n")
                //añadir cada caracter leido a la variable texto (array de 100 caracteres)
                //printf("Cargando el bloque % en la linea %\n", bloque, linea)
                //VolcarCACHE(tbl);
            //else
                //numfallos++;
                //printf("T: %d, Fallo de CACHE, ADDR %04X Label %X linea %02X palabra %02X bloque %02X\n")
                //globaltime+=20;
                //copiar bloque desde el array RAM 
                //printf("Cargando el bloque % en la linea %\n", bloque, linea)
                //actualizar label y 8 bytes de datos de la linea (TratarFallo??)
            //sleep de 1 segundo
        }
        //printf("Accesos totales: %d; fallos: %d; tiempo medio %d\nTexto leido: %");
        //volcar 128 bytes de info (8 lineas de 16 bytes cada una) de la caché en CONTENTS_CACHE.bin
    }

    fclose(fd); //cerrar fd
    return 0;
}

// suponiendo que la funcion limpiarcache es la que asigna los valores 0xFF y 0x23
void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]){
    for(int i=0; i<NUM_FILAS; i++){
        tbl->ETQ=0xFF;
        for(int j=0; j<TAM_LINEA; j++){
            tbl->Data[j]=0x23;
        }
    }
}

// contar la cantidad de lineas que tiene dirs_memoria.txt, no estoy segura si hace falta ya que hay una cantidad de lineas ya definida
int contarLineas(FILE *fd){
    char caract;
    int lineas = 0;

    while((caract = getc(fd))!=EOF){
        if(caract=="\n"){
            lineas++;
        }
    }

    return lineas;
}

void imprimirCACHE(T_CACHE_LINE *tbl){
    for(int i=0; i<8; i++){
        printf("\nc - ", tbl->ETQ);
        for(int j=0; j<TAM_LINEA; j++){
            printf("%c, ", tbl->Data[j]);
        }
    }
}

/* agrego un template que tengo de leeLineaDinamica que a lo mejor sirve para leer los ficheros

char *leeLineaDinamica(){
	char *ptr = (char *)malloc(TAM_BLOQUE);
    char letra;
    int i = 0;
    int m = 1;
    while ((letra = getchar())!='\n'){
        *(ptr + i) = letra;
        i++;
		if (i==TAM_BLOQUE){
            m++;
			ptr = (char*)realloc(ptr,m*TAM_BLOQUE);
		}
    }
    *(ptr+i)='\0';
    return ptr;
}*/
