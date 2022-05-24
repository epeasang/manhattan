int calcularDistancia(int i, int j, int posx, int posy)
{
	return abs(i-posx) + abs(j-posy);
}

void buscarse(__global int *numeros, int index, int posx, int posy,__global int *distancias, int N,__global int *A)
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



__kernel void Manhattan(int N,__global int *A,int n,__global int *numeros,__global int *distancias)
{
	int total = n/get_global_size(0);
	size_t id= get_global_id(0);


	for (int i = id*total; i < (id+1)*total; i++)
	{
		distancias[i]=0;
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