#include <stdio.h>
#include <stdlib.h>
//#include </usr/include/mpi/mpi.h>
#include <mpi.h>
#include <sys/time.h>


typedef int bool;
#define true 1
#define false 0

#define QUIERO_TRABAJO 1

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

/* Used to return the partial soluciones from a sub-tree of the complete search-tree. Returns the soluciones recursively.
 */
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

/* Changes values in nextPos to a valid configuration, ready for a worker. Returns non-valid position (out of auxTablero range) if it has gone through all valid already.
 */
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

/* Master for when number of proc. are greater than the auxTablero.
 */
int recursionMaster(int myRank, int cantProcesos, int tamanioT) {
    
    if (cantProcesos>((tamanioT-1)*(tamanioT-2))) {printf("Not implemented yet! \n");return 0;}
    printf("  ... Entered master process ...   \n");
    int tablero[tamanioT], buf[3], trabajadoresActivos, solucionesEncontradas, pos[2], sender;
    
    solucionesEncontradas=0;
    
    buf[2] = 1; //mientras este en 1, señala q hay trabajo para los workers
    MPI_Status status;
    tablero[0] = 0;
    tablero[1] = 0;
    
    int* ptrPos;
    pos[0] = 0;
    pos[1] = 0;
    ptrPos = pos;
    // inicializa el trabajo para todos los workers
    for (trabajadoresActivos=0; trabajadoresActivos<cantProcesos-1; trabajadoresActivos++) {
        MPI_Recv(0, 0, MPI_INT, MPI_ANY_SOURCE, QUIERO_TRABAJO, MPI_COMM_WORLD, &status);
        sender = status.MPI_SOURCE;  
        
        proxima_posicion(tablero, ptrPos, tamanioT);
        buf[0] = ptrPos[0];
        buf[1] = ptrPos[1];
        
        MPI_Send(&buf, 3, MPI_INT, sender, 0, MPI_COMM_WORLD);
    }
    // Start receiving soluciones and dish out work until none left. Then kill processes
    while (trabajadoresActivos>0) {
        //el master se queda esperando a que cualquier Worker termine de trabajar
        MPI_Recv(&buf, 3, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        //Al recibir el trabajo, el master identifica al worker que termino. 
        sender = status.MPI_SOURCE;   
        //el sender es el worker que termino
        //printf("Worker no. %d found %d soluciones \n ", sender, buf[0]);
        solucionesEncontradas += buf[0];
        proxima_posicion(tablero, ptrPos, tamanioT);
        if (ptrPos[0]<tamanioT){
            buf[0] = ptrPos[0];
            buf[1] = ptrPos[1];
            MPI_Send(&buf, 3, MPI_INT,
             sender, 0, MPI_COMM_WORLD);
        }
        else {
            buf[2] = 0; //ya no hay trabajo
            MPI_Send(&buf, 3, MPI_INT, sender, 0, MPI_COMM_WORLD);
            trabajadoresActivos--;
        }
    }
    printf(" ... Master process terminated ... \n");
    return solucionesEncontradas;
}

/* Worker for when no. processes>tamanioT
 */
void recursionWorker(int myRank, int cantProcesos, int tamanioT) {
    int buf[3], solucionesParciales;
    MPI_Status status;
    //El worker es el que pide trabajo
    MPI_Send(0, 0, MPI_INT, 0, QUIERO_TRABAJO, MPI_COMM_WORLD);
    // Recibe el primer trabajo
    MPI_Recv(&buf, 3, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    // Receive more work until termination message received
    while (buf[2]==1) {// hay trabajo
        solucionesParciales=0;
        int tablero[tamanioT];
        tablero[0] = buf[0];
        tablero[1] = buf[1];
        buf[0] = recursionTablero(2, tablero, tamanioT);
        MPI_Send(&buf, 3, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Recv(&buf, 3, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        
    }
        
  
    return;
}

/* Main. Initalizes MPI, starts a master and some workers, waits results and finalizes MPI. Also times the whole computation.
 */
int main(int argc, char *argv[]) {
    int myRank, cantProcesos, soluciones, tamanioT;
    double tiempoTotal;
    
    if ((argc != 2) || ((tamanioT = atoi(argv[1])) <= 0) ){
        printf("\nUsar: %s n\n  n: Dimension del tablero (nxn X nxn)\n ", argv[0]);
        exit(1);
    }
    
    MPI_Init(&argc, &argv); // inicializa los argumentos del programa
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);  // obtener el identificador de proceso (0..P).
    MPI_Comm_size(MPI_COMM_WORLD, &cantProcesos);  // cantidad de procesos creados
    tiempoTotal= dwalltime();
    if (myRank==0){ // si soy el central
        soluciones = recursionMaster(myRank, cantProcesos, tamanioT); 
    }
    else {  // soy un trabajador
        recursionWorker(myRank, cantProcesos, tamanioT);
    }
    if (myRank==0){
    	printf("Cantidad Soluciones %d\n", soluciones);
    	printf("Tiempo en segundos %f\n", dwalltime() - tiempoTotal);
		}
    MPI_Finalize();
    return 0;
}
