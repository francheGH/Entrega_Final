#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

int N;
double *A,*B,*L,*U,*R1,*R2,*R3,*R;
double aMax=0;
double aMin=99999;
double aProm=0;
double temp1=0;
int T = 2;
pthread_barrier_t barrier1;
pthread_mutex_t mutexMaximo;
pthread_mutex_t mutexMinimo;

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


//Funcion que ejecutan los hilos

void* funcionR(void* id){
    int mi_id = *(int*)id;
    int i, j ,k;
    int x,y;
    int inicio=(mi_id*N)/T;
    int fin=((mi_id+1)*N)/T;

    
    for(x=inicio;x<fin;x++){
        for (y=0; y<N; y++){
            pthread_mutex_lock(&mutexMaximo);
            if (A[x*N+y]>aMax){
                aMax=A[x*N+y];
            }
            pthread_mutex_unlock(&mutexMaximo);
        }
    }

    for(x=inicio;x<fin;x++){
        for (y=0; y<N; y++){
            pthread_mutex_lock(&mutexMinimo);
            if (A[x*N+y]<aMin){
                aMin=A[x*N+y];
            }
            pthread_mutex_unlock(&mutexMinimo);
        }
    }

    for(x=inicio;x<fin;x++){
        for (y=0; y<N; y++){
            temp1=temp1+A[x*N+y];
            
        }
    }

    
    pthread_barrier_wait(&barrier1);
    
    aProm=temp1/(N*N);
    /*
    printf("El minimo es %f\n",aMin);
    printf("El promedio es %f\n",aProm);
     */
    

    /*R1= aMin*A*L */
    for(i=(mi_id*N)/T;i<((mi_id+1)*N)/T;i++){
        for (j=0; j<N; j++){
            for (k=0;k<(i+1); k++){
                    R1[i*N + j] = R1[i*N + j] +  A[i*N + k] * L[k+((j*(j+1))/2)] * aMin;
            }
        }
    }

    /*R2= aMax*A*B */
    for(i=(mi_id*N)/T;i<((mi_id+1)*N)/T;i++){
        for (j=0; j<N; j++){
            for (k=0;k<N; k++){
                    R2[i*N + j] = R2[i*N + j] +  A[i*N + k] * B[k + j*N] * aMax; 
            }
        }
    }
    /*R3= aProm*U*B */
    for(i=(mi_id*N)/T;i<((mi_id+1)*N)/T;i++){
        for (j=0; j<N; j++){
            for (k=0;k<(i+1); k++){
                        R3[i*N + j] = R3[i*N + j] + U[k + (i*(i+1))/2] * B[k+j*N] * aProm;
            }
        }
    }
    pthread_barrier_wait(&barrier1);
    /*R=R1+R2 */
    for(i=(mi_id*N)/T;i<((mi_id+1)*N)/T;i++){
        for(j = 0; j < N; j++){
            R[i*N + j] = R[i*N+j] + R1[i*N + j] + R2[i*N + j]; 
        }
    }
    pthread_barrier_wait(&barrier1);
    /*R=R+R3 */
    for(i=(mi_id*N)/T;i<((mi_id+1)*N)/T;i++){
        for(j = 0; j < N; j++){
            R[i*N + j] = R[i*N+j] + R3[i*N + j];
        }
    }
    pthread_exit(NULL);
}

int main(int argc,char*argv[]){
    int i,j,k;
    double timetick;
    pthread_t threads[T];
    int threads_id[T];

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
    

    timetick = dwalltime();

    pthread_barrier_init(&barrier1,NULL,T); 
    pthread_mutex_init(&mutexMaximo, NULL);
    pthread_mutex_init(&mutexMinimo, NULL);
    


    for(i=0; i<T; i++){
        threads_id[i] = i;
        pthread_create(&threads[i],NULL,&funcionR,(void*)&threads_id[i]);
    }
  
    for (i=0; i<T; i++){
        pthread_join(threads[i],NULL);
    }


    pthread_barrier_destroy(&barrier1);

    pthread_mutex_destroy(&mutexMaximo);
    pthread_mutex_destroy(&mutexMinimo);
    
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