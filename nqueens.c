#include <stdio.h>
#include <stdlib.h>


int *reinas;    //Vector con las posiciones de las reinas
int N;          //Número de reinas
int i;          // Contador

int cantSolutions = 0;

int check(int fila, int reinas[], int n) 
{
  for (i=0; i<fila; i++)
      if ( (reinas[i]==reinas[fila])                      // Misma columna
         || (abs(fila-i) == abs(reinas[fila]-reinas[i]))) // Misma diagonal
        return 0;
  return 1;
}

void insertReina (int fila, int reinas[], int n)
{
  if (fila<n) {
     for (reinas[fila]=0; reinas[fila]<n; reinas[fila]++) {
        if (check(fila,reinas,n)) {
            insertReina (fila+1, reinas, n);
        }
     } 
  } else {
     cantSolutions++;
  }
}

void main (int argc, char *argv[])
{
    int *reinas;  // Vector con las posiciones de las reinas de cada fila
    int N;  // Número de reinas
    int i;        // Contador


    // Leer número de reinas 
    // (parámetro del programa)
    N = -1;

    if ((argc != 2)){
            printf("\nUsar: %s n\n  n: Dimension de la matriz (nxn X nxn)\n", argv[0]);
            exit(1);
        }else{
            N=atoi(argv[1]);
    }


    // Colocar las reinas en el tablero
    if (N>0) {

    // Crear vector dinámicamente
    reinas = (int*) malloc ( sizeof(int)*N );

    // Inicializar vector:
    for (i=0; i<N; i++){
        reinas[i] = -1;
        }
        
    }
    // Colocar reinas (Bactracking)
    insertReina(0,reinas,N);
    printf("Cantidad de soluciones para %dx%d =",N,N);
    printf(" %d \n",cantSolutions);
    free (reinas);

}

/*
Se pueden agregar varias cosas, entre las primeras
    girar en 90 180 270 para encontrar 2 4 y 8 solucione
*/