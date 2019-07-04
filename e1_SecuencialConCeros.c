#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int N;
double *A,*B,*L,*U,*R1,*R2,*R3,*R;

/*
void imprimirMatriz(double *mAux){
    for(int i=0; i<N;i++){
        for(int j = 0; j<N; j++){
            printf("%f  ",mAux[i+j*N]);
            if(j == N-1){
                printf( "\n");
            }
        }
    }
}
 */

//Para calcular tiempo
double dwalltime(){
    double sec;
    struct timeval tv;

    gettimeofday(&tv,NULL);
    sec = tv.tv_sec + tv.tv_usec/1000000.0;
    return sec;
}
//calcula el maximo de una matriz pasada como parametro
double calcularMax(double *mAux){
    double max=0;
    int x,y;
    for(x=0;x<N;x++){
        for (y=0; y<N; y++){
            if (mAux[x*N+y]>max){
                max=mAux[x*N+y];
            }
        }
    }
    return max;
}
//calcula el minimo de una matriz pasada como parametro
double calcularMin(double *mAux){
    double min=99999999;
    int x,y;
    for(x=0;x<N;x++){
        for (y=0; y<N; y++){
            if (mAux[x*N+y]<min){
                min=mAux[x*N+y];
            }
        }
    }
    return min;
}
//calcula el promedio de una matriz pasada como parametro
double calcularProm(double *mAux){
    double prom=0;
    int x,y;
    for(x=0;x<N;x++){
        for (y=0; y<N; y++){
            prom=prom+mAux[x*N+y];
        }
    }
    return (prom/(N*N));
}

int main(int argc,char*argv[]){
    int i,j,k;
    double aMax, aMin, aProm;
    int check=1;
    double timetick;

     //Controla los argumentos al programa
    if ((argc != 2) || ((N = atoi(argv[1])) <= 0) ){
        printf("\nUsar: %s n\n  n: Dimension de la matriz (nxn X nxn)\n", argv[0]);
        exit(1);
    }

    //Aloca memoria para las matrices
    A=(double*)malloc(sizeof(double)*N*N); /*Almacenada por filas*/
    B=(double*)malloc(sizeof(double)*N*N); /*Almacenada por columnas siendo la transpuesta de A*/
    L=(double*)malloc(sizeof(double)*N*N); /*Triangular Inferior almacenada por columnas en arreglo*/
    U=(double*)malloc(sizeof(double)*N*N); /*Triangular Superior almacenada por filas en arreglo*/
    R1=(double*)malloc(sizeof(double)*N*N); /*Almacenada por filas*/
    R2=(double*)malloc(sizeof(double)*N*N); /*Almacenada por filas*/
    R3=(double*)malloc(sizeof(double)*N*N); /*Almacenada por filas*/
    R=(double*)malloc(sizeof(double)*N*N); /*Almacenada por filas*/
    

    //Inicializacion de matrices
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            A[i*N+j]= 1;
            B[i+j*N]= A[i*N+j];
            R1[i*N+j]=0;
            R2[i*N+j]=0;
            R3[i*N+j]=0;
            R[i*N+j]=0;
            
            if (i > j){
				U[j+((i*(i+1))/2)] = 1; 
                L[i+((j*(j+1))/2)] = 0; 
               
		  	}else if (i < j){
				L[i+((j*(j+1))/2)] = 1; 
                U[j+((i*(i+1))/2)] = 0; 
			}else if (i==j){
				U[j+((i*(i+1))/2)] = 1;
				L[i+((j*(j+1))/2)] = 1;
			}
        }
    }

    timetick = dwalltime();

    aProm=calcularProm(A);
    aMax=calcularMax(A);
    aMin=calcularMin(A);

    /*R1= aMin*A*L */
    for(i=0;i<N;i++){
        for (j=0; j<N; j++){
            for (k=0;k<N; k++){
                    R1[i*N + j] = R1[i*N + j] +  A[i*N + k] * L[k+((j*(j+1))/2)] * aMin;
            }
        }
    }
    

    /*R2= aMax*A*B */
    for(i=0;i<N;i++){
        for (j=0; j<N; j++){
            for (k=0;k<N; k++){
                    R2[i*N + j] = R2[i*N + j] +  A[i*N + k] * B[k + j*N] * aMax; 
            }
        }
    }
    

    /*R3= aProm*U*B */
    for(i=0;i<N;i++){
        for (j=0; j<N; j++){
            for (k=0;k<N; k++){
                        R3[i*N + j] = R3[i*N + j] + U[k + (i*(i+1))/2] * B[k+j*N] * aProm;
            }
        }
    }
    
    /*R=R1+R2 */
    for(i = 0; i < N; i++){
        for(j = 0; j < N; j++){
            R[i*N + j] = R[i*N+j] + R1[i*N + j] + R2[i*N + j]; 
        }
    }
    /*R=R+R3 */
    for(i = 0; i < N; i++){
        for(j = 0; j < N; j++){
            R[i*N + j] = R[i*N+j] + R3[i*N + j];
        }
    }

    /*
    printf("Esta es la matriz R \n");
    imprimirMatriz(R);
    */



    printf("Tiempo en segundos %f\n", dwalltime() - timetick);
   
    
    

    free(A);
    free(B);
    free(L);
    free(U);
    free(R1);
    free(R2);
    free(R3);
    free(R);
    return(0);

}