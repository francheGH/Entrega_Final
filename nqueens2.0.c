#include <stdio.h>
#include <stdlib.h>
#include </usr/include/mpi/mpi.h>
//#include <mpi.h>
#include <sys/time.h>
typedef int bool;
#define true 1
#define false 0

double dwalltime(){
    double sec;
    struct timeval tv;

    gettimeofday(&tv,NULL);
    sec = tv.tv_sec + tv.tv_usec/1000000.0;
    return sec;
}

bool chequearPosicion(const int contador, int auxTablero[]) {
    int row_1, row_2;
    for (int col_1=0; col_1<contador; col_1++){
        row_1 = auxTablero[col_1];
        for (int col_2=col_1+1; col_2<contador; col_2++) {
            row_2 = auxTablero[col_2];
            if (row_1==row_2||abs(row_1-row_2)==abs(col_1-col_2)) return false;
        }
    }
    return true;
}
int recursionTablero(int contador, int auxTablero[], int tamanioT){
    int esSolucion = 0;
    if (contador >= tamanioT) {
        if (chequearPosicion(contador,auxTablero)) {
            esSolucion++;
            return esSolucion;
        }
    }
    else if (chequearPosicion(contador,auxTablero)){
        for(int row=0; row<tamanioT; row++){
            auxTablero[contador] = row;
            esSolucion += recursionTablero(contador+1, auxTablero, tamanioT);
        }
    }
    return esSolucion;
}


void proxima_posicion(int auxTablero[], int* nextPos, int tamanioT) {
    // Start by incrementing by one so not to return original position
    //Si no encontre solucion en todas las filas,
    //Aumento en la fila de mi columna anterior
    //Esto significa que no habia soluciones para tablero[0]=filaActual;
    //nota: El contenido de auxTablero es fila y los indices columnas.
    if (auxTablero[1]==tamanioT-1) {
        auxTablero[0]++; 
        auxTablero[1]=0;
    }
    else auxTablero[1]++;
    // Then keep changing as long as its not valid
    while (!chequearPosicion(2,auxTablero)) {
        if (auxTablero[0] == tamanioT) break;
        else if (auxTablero[1]>=tamanioT-1) {
            auxTablero[0]++;
            auxTablero[1]=0;
        }
        else auxTablero[1]++;
    }
    nextPos[0] = auxTablero[0];
    nextPos[1] = auxTablero[1];
}

int recursionB(int tamanioT, int *pos) {
    int solucionesParciales;     
    int tablero[tamanioT];
    tablero[0] = pos[0];
    tablero[1] = pos[1];
    solucionesParciales=recursionTablero(2, tablero, tamanioT);
    return solucionesParciales;
}
int recursion(int tamanioT) { 
    int tablero[tamanioT], solucionesEncontradas, pos[2]; 
    solucionesEncontradas=0;
    tablero[0] = 0;
    tablero[1] = 0;
    
    int* ptrPos;
    pos[0] = 0;
    pos[1] = 0;
    ptrPos = pos;
    proxima_posicion(tablero, ptrPos, tamanioT);
    while (ptrPos[0]<tamanioT){
        solucionesEncontradas+=recursionB(tamanioT,ptrPos);
        proxima_posicion(tablero,ptrPos,tamanioT);
    }
    return solucionesEncontradas;

}
int main(int argc, char *argv[]) {
    int soluciones, tamanioT;
    double tiempoTotal;
    if ((argc != 2) || ((tamanioT = atoi(argv[1])) <= 0) ){
        printf("\nUsar: %s n\n  n: Dimension del tablero (nxn X nxn)\n ", argv[0]);
        exit(1);
    }
    tiempoTotal= dwalltime();
    soluciones = recursion(tamanioT); 
    printf("Cantidad Soluciones %d\n", soluciones);
    printf("Tiempo en segundos %f\n", dwalltime() - tiempoTotal);
    return 0;
}