#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <cmath>

void initialize(int *m,int t,int maximo)
{
  int i;
  for(i=0;i<t*t;i++)
  {  
    m[i] =(int) (((1.*rand())/RAND_MAX)*maximo);
  }
}

void escribir(int *m, int t){
  int i, j;

  for (i = 0; i < t; i++)
  {
    for (j = 0; j < t; j++)
    {  
      printf("%d ",m[i*t+j]);
    }
    printf("\n");
  }
  printf("\n");
}

void escribirresult(int *m, int t){
  int i;

  for (i = 0; i < t; i++)
  {
    printf("%d ",m[i]);
  }
  printf("\n");
}

/*
c
c     mseconds - returns elapsed milliseconds since Jan 1st, 1970.
c
*/
long long mseconds(){
  struct timeval t;
  gettimeofday(&t, NULL);
  return t.tv_sec*1000 + t.tv_usec/1000;
}

int ObtenerParametros(int argc, char *argv[], int *debug) {
	int i;
	*debug=0;
	if (argc<2)
		return 0;
	for (i=2; i<argc;) {
		if (strcmp(argv[i], "-d")==0) {
			*debug=1;
			i++;
		}
		else
			return 0;
	}
	return 1;
}

// **************************************************************************
// ***************************** IMPLEMENTACIÓN *****************************
// **************************************************************************
/*
N -> Tamaño de la matriz A (NxN)
A -> Matriz
n -> Cantidad de enteros contenidos en numeros
numeros -> Lista de números de los que se buscarán las distancias
distancias -> Resultado con las distancias (un resultado por número)
*/

int calcularDistancia(int i, int j, int posx, int posy)
{
	return abs(i-posx) + abs(j-posy);
}

void buscarse(int *numeros, int index, int posx, int posy, int *distancias, int N, int *A)
{
	for (int i = posx; i < N; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			if (A[i*N + j] == numeros[index])
			{
				int distancia = calcularDistancia(i, j, posx, posy);
				if (distancia > distancias[index])
				{
					distancias[index] = distancia;
				}
			}
		}
	}
}




void sec(int N,int *A,int n,int *numeros,int *distancias)
{
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			for (int k = 0; k < N; ++k)
			{
				if (A[j*N + k] == numeros[i])
				{
					buscarse(numeros,i,j,k, distancias, N, A);
				}
			}
		}
	}
}
// **************************************************************************
// *************************** FIN IMPLEMENTACIÓN ***************************
// **************************************************************************

/*
Recibirá los siguientes parámetros (los parámetros entre corchetes son opcionales): fichEntrada [-d]
fichEntrada -> Obligatorio. Fichero de entrada con los parámetros de lanzamiento de los experimentos
-d -> Opcional. Si se indica, se mostrarán por pantalla los valores iniciales, finales y tiempo de cada experimento
*/
int main(int argc,char *argv[]) {
	int i,j,
		debug=0,		  // Indica si se desean mostrar los tiempos y resultados parciales de los experimentos
		N,			  // Tamaño de la matriz A (NxN)
		cuantos,		  // Número de experimentos
		n,			  // Número de enteros a leer del fichero
		semilla,		  // Semilla para la generación de números aleatorios
		maximo,		  // Valor máximo de para la generación de valores aleatorios de la matriz A (se generan entre 0 y maximo-1)
		*A,			  // Matriz A. Se representa en forma de vector. Para acceder a la fila f y la columna c: A[f*N+c]
		*numeros,		  // Vector que contiene los números de los que se calcularán las distancias
		*distancias;	  // Vector resultado conteniendo las distancias calculadas
	long long ti,		  // Tiempo inicial
			tf,		  // Tiempo final
			tt=0; 	  // Tiempo acumulado de los tiempos parciales de todos los experimentos realizados
	FILE *f;			  // Fichero con los datos de entrada
	
	if (!ObtenerParametros(argc, argv, &debug)) {
		printf("Ejecución incorrecta\nEl formato correcto es %s fichEntrada [-d]\n", argv[0]);
		return 0;
	}
	
	// Se leen el número de experimentos a realizar
	f=fopen(argv[1],"r");
	fscanf(f, "%d",&cuantos);

	for(i=0;i<cuantos;i++)
	{
		//Por cada experimento se leen
		fscanf(f, "%d",&N);			//El tamaño de la matriz (NxN)
		fscanf(f, "%d",&semilla);	//La semilla para la generación de números aleatorios
		fscanf(f, "%d",&maximo);		//El valor máximo de para la generación de valores aleatorios de la matriz A (se generan entre 0 y maximo-1)
		fscanf(f, "%d",&n);			//El número de enteros a leer del fichero
		//Reserva de memoria para la matriz, las distancias y los números
		A = (int *) malloc(sizeof(int)*N*N);
		distancias=(int *) malloc(sizeof(int)*n);
		numeros = (int *) malloc(sizeof(int)*n);
		//Lectura de los números
		for(j=0;j<n;j++) {
			fscanf(f, "%d",&numeros[j]);
		}

		srand(semilla);
		initialize(A,N,maximo);

		if (debug)	{
			printf("Matriz del experimento %d:\n", i); escribir(A,N);
			printf("Números del experimento %d:\n", i);escribirresult(numeros,n);
		}
   
		ti=mseconds(); 
		sec(N,A,n,numeros,distancias);
		tf=mseconds(); 
		tt+=tf-ti;

		if (debug)	{
			printf("Tiempo del experimento %d: %Ld ms\n", i, tf-ti);
			printf("Resultado del experimento %d:\n", i); escribirresult(distancias,n);
		}

		free(A);
		free(numeros);
		free(distancias);
	}
	printf("Tiempo total de %d experimentos: %Ld ms\n", cuantos, tt);
	fclose(f);

  return 0;
}
