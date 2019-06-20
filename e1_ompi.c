#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <pthread.h>
#include <omp.h>

int N;
double *A,*B,*L,*U,*R1,*R2,*R3,*R;
int N=100;
int T;
double aMax=0;
double aMin=99999;
double aProm=0;
double temp1=0;


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

//Para calcular tiempo
double dwalltime(){
    double sec;
    struct timeval tv;

    gettimeofday(&tv,NULL);
    sec = tv.tv_sec + tv.tv_usec/1000000.0;
    return sec;
}

int main(int argc,char*argv[]){
    int i,j,k;
    int x,y;
    double timetick;
    

     //Controla los argumentos al programa
    if ((argc != 3) || ((N = atoi(argv[1])) <= 0) || ((T = atoi(argv[2])) <= 0)){
        printf("\nUsar: %s n\n  n: Dimension de la matriz (nxn X nxn)\n , y seleccionar cantidad de Hilos", argv[0]);
        exit(1);
    }


    //Aloca memoria para las matrices
    A=(double*)malloc(sizeof(double)*N*N); /*Almacenada por filas*/
    B=(double*)malloc(sizeof(double)*N*N); /*Almacenada por columnas siendo la transpuesta de A*/
    L=(double*)malloc(sizeof(double)*((N*(N+1))/2)); /*Triangular Inferior almacenada por columnas en arreglo*/
    U=(double*)malloc(sizeof(double)*((N*(N+1))/2)); /*Triangular Superior almacenada por filas en arreglo*/
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
		  	}else if (i < j){
				L[i+((j*(j+1))/2)] = 1; 
			}else if (i==j){
				U[j+((i*(i+1))/2)] = 1;
				L[i+((j*(j+1))/2)] = 1;
			}
        }
    }

    omp_set_num_threads(T);


    timetick = dwalltime();

    #pragma omp parallel
    {
        #pragma omp for schedule(dynamic,T) private(x,y) 
            for(x=0;x<N;x++){
                for (y=0; y<N; y++){
                    if (A[x*N+y]<aMin){
                        aMin=A[x*N+y];
                    }
                }
            }
       
        #pragma omp for schedule(dynamic,T) private(i,j,k) nowait
            for(i=0;i<N;i++){
                for (j=0; j<N; j++){
                    for (k=0;k<(i+1);k++){
                        R1[i*N + j] = R1[i*N + j] +  A[i*N + k] * L[k+((j*(j+1))/2)] * aMin;
                        
                    }
                }
            }
        #pragma omp for schedule(dynamic,T) private(x,y) 
            for(x=0;x<N;x++){
                for (y=0; y<N; y++){
                    if (A[x*N+y]>aMax){
                        aMax=A[x*N+y];
                    }
                }
            }
        
        #pragma omp for schedule(dynamic,T) private(i,j,k) nowait
            for(i=0;i<N;i++){
                for (j=0; j<N; j++){
                    for (k=0;k<N; k++){
                        R2[i*N + j] = R2[i*N + j] +  A[i*N + k] * B[k + j*N] * aMax;
                        //printf("\nR2[%d,%d] = %f \n",i, j, R2[i*N + j]);
                    }
                }
            }
        #pragma omp for schedule(dynamic,T) private(i,j) reduction(+ : temp1) nowait
            for (i=0; i<N; i++){
                for (j= 0; j<N; j++){
                    temp1 += A[i*N+j];
                }
            }

        #pragma omp single
            aProm= temp1/(N*N);

        #pragma omp for schedule(dynamic,T) private(i,j,k) nowait
            for(i=0;i<N;i++){
                for (j=0; j<N; j++){
                    for (k=0;k<(i+1); k++){
                        R3[i*N + j] = R3[i*N + j] + U[k+(i*(i+1))/2] * B[k+j*N] * aProm;
                        //printf("\nR3[%d,%d] = %f \n",i, j, R3[i*N + j]);
                    }
                }
            }
        #pragma omp for schedule(dynamic,T) private(i,j) 
            for(i = 0; i < N; i++){
                for(j = 0; j<N; j++){
                    R[i*N + j] = R[i*N+j] + R1[i*N + j] + R2[i*N + j]; 
                }
            }
        #pragma omp for schedule(dynamic,T) private(i,j) 
            for(i = 0; i < N; i++){
                for(j = 0; j < N; j++){
                    R[i*N + j] = R[i*N+j] + R3[i*N + j];
                }
            }
       
    }
    

    
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





