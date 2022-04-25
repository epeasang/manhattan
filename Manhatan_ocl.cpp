#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <CL/cl.h>

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

int ObtenerParametros(int argc, char *argv[], int *debug, int *num_workitems, int *workitems_por_workgroups) {
	int i;
	*debug=0;
	*num_workitems=0;
	*workitems_por_workgroups=0;
	if (argc<2)
		return 0;
	for (i=2; i<argc;) {
		if (strcmp(argv[i], "-d")==0) {
			*debug=1;
			i++;
		}
		else if (strcmp(argv[i], "-wi")==0) {
			i++;
			if (i==argc)
				return 0;
			*num_workitems=atoi(argv[i]);
			i++;
			if (*num_workitems<=0)
				return 0;
		}
		else if (strcmp(argv[i], "-wi_wg")==0) {
			i++;
			if (i==argc)
				return 0;
			*workitems_por_workgroups=atoi(argv[i]);
			i++;
			if (*workitems_por_workgroups<=0)
				return 0;
		}
		else
			return 0;
	}
	return 1;
}

typedef struct {
	int num_plataformas, num_dispositivos;
	cl_platform_id *plataformas;
	cl_device_id *dispositivos;
	cl_context contexto;
	cl_command_queue cola;
	cl_program programa;
	cl_kernel kernel; // Se pueden definir más campos de tipo cl_kernel si es necesario
} EntornoOCL_t;

// **************************************************************************
// ***************************** IMPLEMENTACIÓN *****************************
// **************************************************************************

//Codigos de error
void CodigoError(cl_int err)
{
	switch (err) {
		case CL_SUCCESS:
            printf("Ejecución correcta\n");
        break;
		case CL_BUILD_PROGRAM_FAILURE:
            printf("CL_BUILD_PROGRAM_FAILURE\n");
        break;
		case CL_COMPILER_NOT_AVAILABLE:
            printf("CL_COMPILER_NOT_AVAILABLE\n");
        break;
		case CL_DEVICE_NOT_AVAILABLE:
            printf("CL_DEVICE_NOT_AVAILABLE \n");
        break;
		case CL_DEVICE_NOT_FOUND:
            printf("CL_DEVICE_NOT_FOUND\n");
        break;
		case CL_INVALID_ARG_INDEX:
            printf("CL_INVALID_ARG_INDEX \n");
        break;
		case CL_INVALID_ARG_SIZE:
            printf("CL_INVALID_ARG_SIZE \n");
        break;
		case CL_INVALID_ARG_VALUE:
            printf("CL_INVALID_ARG_VALUE\n");
        break;
		case CL_INVALID_BUFFER_SIZE:
            printf("CL_INVALID_BUFFER_SIZE \n");
        break;
		case CL_INVALID_BUILD_OPTIONS:
            printf("CL_INVALID_BUILD_OPTIONS\n");
        break;
		case CL_INVALID_COMMAND_QUEUE:
            printf("CL_INVALID_COMMAND_QUEUE \n");
        break;
		case CL_INVALID_CONTEXT:
            printf("CL_INVALID_CONTEXT\n");
        break;
		case CL_INVALID_DEVICE_TYPE:
            printf("CL_INVALID_DEVICE_TYPE\n");
        break;
		case CL_INVALID_EVENT:
            printf("CL_INVALID_EVENT\n");
        break;
		case CL_INVALID_EVENT_WAIT_LIST:
            printf("CL_INVALID_EVENT_WAIT_LIST \n");
        break;
		case CL_INVALID_GLOBAL_WORK_SIZE:
            printf("CL_INVALID_GLOBAL_WORK_SIZE \n");
        break;
		case CL_INVALID_HOST_PTR:
            printf("CL_INVALID_HOST_PTR \n");
        break;
		case CL_INVALID_KERNEL:
            printf("CL_INVALID_KERNEL \n");
        break;
		case CL_INVALID_KERNEL_ARGS:
            printf("CL_INVALID_KERNEL_ARGS \n");
        break;
		case CL_INVALID_KERNEL_NAME:
            printf("CL_INVALID_KERNEL_NAME\n");
        break;
		case CL_INVALID_MEM_OBJECT:
            printf("CL_INVALID_MEM_OBJECT \n");
        break;
		case CL_INVALID_OPERATION:
            printf("\n");
        break;
		case CL_INVALID_PLATFORM:
            printf("CL_INVALID_PLATFORM\n");
        break;
		case CL_INVALID_PROGRAM:
            printf("CL_INVALID_PROGRAM \n");
        break;
		case CL_INVALID_PROGRAM_EXECUTABLE:
            printf("CL_INVALID_PROGRAM_EXECUTABLE\n");
        break;
		case CL_INVALID_QUEUE_PROPERTIES:
            printf("CL_INVALID_QUEUE_PROPERTIES\n");
        break;
		case CL_INVALID_VALUE: 
            printf("CL_INVALID_VALUE\n");
        break;
		case CL_INVALID_WORK_DIMENSION:
            printf("CL_INVALID_WORK_DIMENSION \n");
        break;
		case CL_INVALID_WORK_GROUP_SIZE:
            printf("CL_INVALID_WORK_GROUP_SIZE \n");
        break;
		case CL_INVALID_WORK_ITEM_SIZE:
            printf("CL_INVALID_WORK_ITEM_SIZE \n");
        break;
		case CL_MEM_OBJECT_ALLOCATION_FAILURE:
            printf("CL_MEM_OBJECT_ALLOCATION_FAILURE \n");
        break;
		case CL_OUT_OF_HOST_MEMORY:
            printf("CL_OUT_OF_HOST_MEMORY\n");
        break;
		case CL_OUT_OF_RESOURCES:
            printf("CL_OUT_OF_RESOURCES\n"); 
        break;
		case CL_PROFILING_INFO_NOT_AVAILABLE:
            printf("CL_PROFILING_INFO_NOT_AVAILABLE \n");
        break;
		default:
            printf("Código de error no contemplado\n"); 
        break;
	}
}

cl_int ObtenerPlataformas(cl_platform_id *&platforms, cl_uint &num_platforms)
{
    cl_int error;
    int i;
    char Info[500];
    error = clGetPlatformIDs(0, NULL, &num_platforms);
    if (error!=CL_SUCCESS){
        CodigoError(error);
        return error;
    }
    platforms = new cl_platform_id[num_platforms];
    error = clGetPlatformIDs(num_platforms, platforms, &num_platforms);
    if (error!=CL_SUCCESS){
        CodigoError(error);
    return error;
    }


    for(i=0; i<num_platforms; i++){
    error = clGetPlatformInfo (platforms[i], CL_PLATFORM_NAME, 500, Info, NULL);
    if (error!=CL_SUCCESS){
        CodigoError(error);
        return error;
    }
    printf("%s\n", Info);
    }
    return CL_SUCCESS;
}

cl_int ObtenerDispositivos(cl_platform_id platform, cl_device_type device_type, cl_device_id *&devices, cl_uint &num_devices)
{

    cl_int error;
    int i;
    char buffer[500];

    error = clGetDeviceIDs(platform,device_type, 0,NULL, &num_devices);
    if (error!=CL_SUCCESS){
        CodigoError(error);
        return error;
    }
    devices = new cl_device_id[num_devices];

    error = clGetDeviceIDs (platform,device_type, num_devices,devices, NULL);
    if (error!=CL_SUCCESS){
        CodigoError(error);
        return error;
    }
    for(i=0;i<num_devices;i++){
       error= clGetDeviceInfo (devices[i], CL_DEVICE_NAME, 500, buffer,NULL);
        if (error!=CL_SUCCESS){
            CodigoError(error);
            return error;
        }
        printf("\t%s\n",buffer);
    }

    return CL_SUCCESS;
}

cl_int CrearContexto(cl_platform_id platform, cl_device_id *devices,cl_uint num_devices, cl_context &contexto)
{
    cl_int error;
    cl_context_properties prop[3]={CL_CONTEXT_PLATFORM, (cl_context_properties) platform, 0};

    contexto=clCreateContext (prop,num_devices, devices, NULL, NULL, &error);
    if (error!=CL_SUCCESS){
        CodigoError(error);
        return error;
    }
    return CL_SUCCESS;
}

char leerFuentes(char *&src, const char *fileName)
{
    FILE *file = fopen(fileName, "rb");
    if (!file)
    {
        printf("Error al abrir el fichero '%s'\n", fileName);
        return 0;
    }

    if (fseek(file, 0, SEEK_END))
    {
        printf("Error de posicionamiento en el '%s'\n", fileName);
        fclose(file);
        return 0;
    }

    long size = ftell(file);
    if (size == 0)
    {
        printf("Error al calcular el tamaño del fichero '%s'\n", fileName);
        fclose(file);
        return 0;
    }

    rewind(file);
    src = (char *)malloc(sizeof(char) * size + 1);
    size_t res = fread(src, 1, sizeof(char) * size, file);
    if (res != sizeof(char) * size)
    {
        printf("Error de lectura del fichero '%s'\n", fileName);
        fclose(file);
        free(src);
        return 0;
    }
    src[size] = '\0';
    fclose(file);
    return 1;
}

cl_int CrearPrograma(cl_program &program, cl_context context,cl_uint num_devices, const cl_device_id *device_list, const char *options, const char *fichero)
{
    cl_int error;
    char CompLog[20000], *fuente;
    leerFuentes(fuente, fichero);

    program=clCreateProgramWithSource(context, 1, (const char **)&fuente, NULL, &error);
    if (error!=CL_SUCCESS){
        CodigoError(error);
        return error;
    }
    error=clBuildProgram(program, num_devices,device_list, options, NULL,NULL);
    if (error!=CL_SUCCESS){
        if(error = CL_BUILD_PROGRAM_FAILURE){
            error=clGetProgramBuildInfo(program, device_list[0], CL_PROGRAM_BUILD_LOG, 20000, CompLog, NULL);
            if (error!=CL_SUCCESS){
                CodigoError(error);
                return error;
            }
            printf("Errores de compilacion: \n%s\n", CompLog);
        }else {
            CodigoError(error);
            return error;
        }
    }

    return CL_SUCCESS;
}

cl_int InicializarEntornoOCL(EntornoOCL_t *entorno)
{
	cl_uint num_plataformas,num_dispositivos;
	cl_int error;
	ObtenerPlataformas(entorno->plataformas, num_plataformas);
	ObtenerDispositivos(entorno->plataformas[0], CL_DEVICE_TYPE_ALL, entorno->dispositivos, num_dispositivos);
	CrearContexto(entorno->plataformas[0], entorno->dispositivos, num_dispositivos, entorno->contexto);
	entorno->cola = clCreateCommandQueue(entorno->contexto, entorno->dispositivos[0], CL_QUEUE_PROFILING_ENABLE, &error);
	if (error != CL_SUCCESS){
		CodigoError(error);
		return error;
	}

	
	CrearPrograma(entorno->programa, entorno->contexto, num_dispositivos, entorno->dispositivos, "", "kernel.cl");
	
	entorno->kernel = clCreateKernel(entorno->programa, "Manhatan", &error);
	if (error != CL_SUCCESS){
		CodigoError(error);
		return error;
	}

	return CL_SUCCESS;
}

cl_int LiberarEntornoOCL(EntornoOCL_t *entorno) 
{
	cl_int error;
	error = clReleaseContext(entorno->contexto);
	if (error != CL_SUCCESS){
		CodigoError(error);
		return error;
	}
	error = clReleaseCommandQueue(entorno->cola);
	if (error != CL_SUCCESS){
		CodigoError(error);
		return error;
	}
	error = clReleaseProgram(entorno->programa);
	if (error != CL_SUCCESS){
		CodigoError(error);
		return error;
	}
	error = clRetainKernel(entorno->kernel);
	if (error != CL_SUCCESS){
		CodigoError(error);
		return error;
	}
	error = clReleaseDevice(entorno->dispositivos[0]);
		CodigoError(error);
	if (error != CL_SUCCESS){
		return error;
	}
	
	return CL_SUCCESS;
}

/*
N -> Tamaño de la matriz A (NxN)
A -> Matriz
n -> Cantidad de enteros contenidos en numeros
numeros -> Lista de números de los que se buscarán las distancias
distancias -> Resultado con las distancias (un resultado por número)
entorno -> Entorno OpenCL
num_workitems -> Número de work items que se usarán para lanzar el kernel. Es opcional, se puede usar o no dentro de la función
workitems_por_workgroups -> Número de work items que se lanzarán en cada work group. Es opcional, se puede usar o no dentro de la función
*/
void ocl(int N,int *A,int n,int *numeros,int *distancias, EntornoOCL_t entorno, int num_workitems, int workitems_por_workgroups) 
{

}
// **************************************************************************
// *************************** FIN IMPLEMENTACIÓN ***************************
// **************************************************************************

/*
Recibirá los siguientes parámetros (los parámetros entre corchetes son opcionales): fichEntrada [-d] [-wi work_items] [-wi_wg workitems_por_workgroup]
fichEntrada -> Obligatorio. Fichero de entrada con los parámetros de lanzamiento de los experimentos
-d -> Opcional. Si se indica, se mostrarán por pantalla los valores iniciales, finales y tiempo de cada experimento
-wi work_items -> Opcional. Si se indica, se lanzarán tantos work items como se indique en work_items (para OpenCL)
-wi_wg workitems_por_workgroup -> Opcional. Si se indica, se lanzarán tantos work items en cada work group como se indique en WorkItems_por_WorkGroup (para OpenCL)
*/
int main(int argc,char *argv[]) {
	int i,j,
		debug=0,				   // Indica si se desean mostrar los tiempos y resultados parciales de los experimentos
		num_workitems=0, 		   // Número de work items que se utilizarán
		workitems_por_workgroups=0, // Número de work items por cada work group que se utilizarán
		N,					   // Tamaño de la matriz A (NxN)
		cuantos,				   // Número de experimentos
		n,					   // Número de enteros a leer del fichero
		semilla,				   // Semilla para la generación de números aleatorios
		maximo,				   // Valor máximo de para la generación de valores aleatorios de la matriz A (se generan entre 0 y maximo-1)
		*A,					   // Matriz A. Se representa en forma de vector. Para acceder a la fila f y la columna c: A[f*N+c]
		*numeros,				   // Vector que contiene los números de los que se calcularán las distancias
		*distancias;			   // Vector resultado conteniendo las distancias calculadas
	long long ti,				   // Tiempo inicial
			tf,				   // Tiempo final
			tt=0; 			   // Tiempo acumulado de los tiempos parciales de todos los experimentos realizados
	FILE *f;					   // Fichero con los datos de entrada
	EntornoOCL_t entorno; 		   // Entorno para el control de OpenCL

	if (!ObtenerParametros(argc, argv, &debug, &num_workitems, &workitems_por_workgroups)) {
		printf("Ejecución incorrecta\nEl formato correcto es %s fichEntrada [-d] [-wi work_items] [-wi_wg workitems_por_workgroup]\n", argv[0]);
		return 0;
	}
	
	InicializarEntornoOCL(&entorno);

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
		ocl(N,A,n,numeros,distancias,entorno, num_workitems, workitems_por_workgroups);
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

	LiberarEntornoOCL(&entorno);
	printf("Tiempo total de %d experimentos: %Ld ms\n", cuantos, tt);
	fclose(f);

	return 0;
}
