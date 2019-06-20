#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int N;
int *queens;
int columna;
int temp1=0;
int temp2=0;
int temp3=0;

bool chequearReina(int*queens, int columna){
  temp1=queens[columna]-columna;
  temp2=queens[columna]+columna;
  temp3=queens[columna];
  for (i=0;i<N;i++){
    if(queens[i]==temp3){
      return 0;//misma columna
    }
    if ((queens[i]-i)==temp1)||(queens[i]+i)==temp2){ 
      return 0;//misma diagonal
    }
  }

  return 1;
}
void inicializarReinas(){
  for (i=0; i<N;i++){
    for (j=0;j<N;j++)
      if (chequearReina(queens, i)){
        queensSolutions[i]=j;
        
      }
    }
  }
}



int main(int argc,char*argv[]){
    
  if ((argc != 2) || ((N = atoi(argv[1])) <= 0) ){
    printf("\nUsar: %s n\n  n: Dimension de la matriz (nxn X nxn)\n ", argv[0]);
    exit(1);
  }
    
}



