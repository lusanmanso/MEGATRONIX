#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUM_FILAS 8
#define TAM_LINEA 16

// NO CAMBIAR
typedef struct {
    unsigned char ETQ;
    unsigned char Data[TAM_LINEA];
} T_CACHE_LINE;

// Array de 4096 unsignedchar
unsigned char Simul_RAM[4096];
int globaltime = 0;
int numfallos = 0;

void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]);
void VolcarCACHE(T_CACHE_LINE *tbl);
void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque);
void TratarFallo(T_CACHE_LINE *tbl, unsigned char *MRAM, int ETQ, int linea, int bloque);

// Asigna xFF a label y x23 a datos cache
void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]) {
   for (int i = 0; i < NUM_FILAS; i++) {
      tbl[i].ETQ = 0xFF;
      for (int j = 0; j < TAM_LINEA; j++) {
         tbl[i].Data[j] = 0x23;
      }
   }
}

// El proceso vuelca el contenido de la caché por pantalla con el siguiente formato:
void VolcarCACHE(T_CACHE_LINE *tbl) {
    for (int i = 0; i < NUM_FILAS; i++) {
        printf("Línea %02X: ", i);
        for (int j = 0; j < TAM_LINEA; j++) {
            printf("%02X ", tbl[i].Data[j]);
        }
        printf("\n");
    }
}

void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque) {
    *ETQ = (addr >> 8) & 0x1F;
    *palabra = (addr >> 4) & 0xF;
    *linea = (addr >> 1) & 0x7;
    *bloque = addr & 0x1;
}

void TratarFallo(T_CACHE_LINE *tbl, unsigned char *MRAM, int ETQ, int linea, int bloque) {
    numfallos++; // Incrementa el valor de numfallos
    printf("T: %d, Fallo de CACHE %d, ADDR %04X Label %X linea %02X palabra %02X bloque %02X\n",
           globaltime, numfallos, (ETQ << 8) | (linea << 1) | bloque, tbl[linea].ETQ, linea, 0, bloque);

    // Se copia el bloque correspondiente desde el array RAM y se imprime un mensaje indicando que se está
    // cargando el bloque X en la línea Y.
    int inicio_bloque = bloque * TAM_LINEA; // Corregir el cálculo del inicio del bloque
    for (int i = 0; i < TAM_LINEA; i++) {
        tbl[linea].Data[i] = MRAM[inicio_bloque + i];
    }

    printf("Cargando el bloque %02X en la línea %02X\n", bloque, linea);

    // Se actualizan tanto el campo Label como los 8 bytes de datos de la línea.
    tbl[linea].ETQ = ETQ;
}

// MAIN
int main() {
    
    // Lee el fichero binario CONTENTS_RAM.bin en la variable Simul_RAM
    FILE *ram_file = fopen("CONTENTS_RAM.bin", "rb");
    // Manejo de errores
    if (ram_file == NULL) {
        perror("Error al abrir el archivo CONTENTS_RAM.bin");
        return -1;
    }

    // Inicializar la RAM
    fread(Simul_RAM, sizeof(unsigned char), sizeof(Simul_RAM), ram_file);
    fclose(ram_file);

    // Lectura del fichero de texto accesos_memoria.txt
    FILE *mem_file = fopen("accesos_memoria.txt", "r"); // Contiene una lista de direcciones de memoria en hexadecimal, una por línea.
    // Manejo de errores
    if (mem_file == NULL) {
        perror("Error al abrir el archivo accesos_memoria.txt");
        return -1;
    }

    // MEMsym lee una dirección del fichero accesos_memoria.txt
    T_CACHE_LINE CACHE[NUM_FILAS];
    // Array de 100 caracteres como máximo (no hace falta usar memoria dinámica).
    char texto[100];

    // Inicializar la caché y otros elementos según las especificaciones
    LimpiarCACHE(CACHE);

    // Procesar direcciones de memoria
    unsigned int addr;

    int ETQ, palabra, linea, bloque;

    while (fscanf(mem_file, "%x", &addr) == 1) {

        ParsearDireccion(addr, &ETQ, &palabra, &linea, &bloque);
        // Obtiene el número de línea y comprueba si la etiqueta de la dirección es igual a
        // Label de la línea de la caché.
        if (CACHE[linea].ETQ != ETQ) {
            TratarFallo(CACHE, Simul_RAM, ETQ, linea, bloque);
        } else {
            // Por pantalla se escribe “T: %d, Acierto de CACHE, ADDR %04X Label%X linea %02X palabra %02X DATO %02X”.
            printf("T: %d, Acierto de CACHE, ADDR %04X Label %X linea %02X palabra %02X DATO %02X\n",
                globaltime, addr, ETQ, linea, palabra, CACHE[linea].Data[palabra]);
        }

        globaltime += 20; // Actualiza tiempo
        usleep(1000000); // Sleep 1 segundo
        printf("Contenido de la caché después del acceso:\n");
        VolcarCACHE(CACHE);
        printf("\n");

        // Construir texto
        if (strlen(texto) < 99) {
            texto[strlen(texto)] = CACHE[linea].Data[palabra];
        }

        // Actualizar bloque
        bloque = addr & 0x1;
    }

    fclose(mem_file);

    printf("Número total de accesos: %d\n", globaltime / 20);
    printf("Número de fallos: %d\n", numfallos);
    printf("Tiempo medio de acceso: %.2f segundos\n", (float)globaltime / (float)(globaltime / 20));

    // Imprimir texto leído
    printf("Texto leído desde la caché: %s\n", texto);

    // Volcar contenidos de la caché en el archivo logscache.txt
    FILE *cacheFile = fopen("logfile.txt", "wb");
    if (cacheFile != NULL) {
        fwrite(CACHE, sizeof(T_CACHE_LINE), NUM_FILAS, cacheFile);
        fclose(cacheFile);
    }

    return 0;
}